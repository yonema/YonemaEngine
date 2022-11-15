#include "FBXRendererAssimp.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../GraphicsEngine.h"
#include "../Utils/StringManipulation.h"
#include "../Utils/AlignSize.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			namespace
			{
				static void AiMatrixToMyMatrix(
					const aiMatrix4x4& srcMatrix, nsMath::CMatrix* dstMatrix)
				{
					for (int y = 0; y < 4; y++)
					{
						for (int x = 0; x < 4; x++)
						{
							dstMatrix->m_fMat[y][x] = srcMatrix[y][x];
						}
					}
					dstMatrix->Transpose();
					return;
				}
			}

			struct SLocalTransform
			{
				aiVector3D Scaling;
				aiQuaternion Rotation;
				aiVector3D Translation;
			};

			void CFBXRendererAssimp::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				// ○定数バッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* modelDescHeaps[] = { m_modelDH.Get() };
				commandList->SetDescriptorHeaps(1, modelDescHeaps);
				auto descriptorHeapH = m_modelDH.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);


				// マテリアルごとに描画
				// 最初にマテリアルごとにメッシュ分解しているため、メッシュごとに描画と同意。
				const unsigned int numMeshes = static_cast<unsigned int>(m_indexBuffers.size());
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* indexBuffer = m_indexBuffers.at(meshIdx);
					const auto* vertexBuffer = m_vertexBuffers.at(meshIdx);
					const auto numIndeices = m_numIndicesArray.at(meshIdx);
					const auto& materialName = m_materialNameTable.at(meshIdx);
					const auto* materialDH = m_materialDHs.at(materialName);

					ID3D12DescriptorHeap* materialDescHeaps[] = { materialDH->Get() };
					commandList->SetDescriptorHeaps(1, materialDescHeaps);
					descriptorHeapH = materialDH->GetGPUHandle();
					commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);

					commandList->SetIndexBuffer(*indexBuffer);
					commandList->SetVertexBuffer(*vertexBuffer);

					commandList->DrawInstanced(numIndeices);

				}
				return;
			}

			void CFBXRendererAssimp::UpdateWorldMatrix(
				const nsMath::CVector3& position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale
			)

			{
				// ワールド行列作成。
				nsMath::CMatrix mTrans, mRot, mScale, mWorld;
				mTrans.MakeTranslation(position);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				mWorld = m_bias * mScale * mRot * mTrans;

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				return;
			}

			void CFBXRendererAssimp::UpdateAnimation(float deltaTime)
			{
				m_animationTimer += deltaTime;

				GetBoneTransforms(m_animationTimer, &m_boneMatrices);

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());

				copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCB + 2);

				return;
			}

			CFBXRendererAssimp::CFBXRendererAssimp(const SModelInitData& modelInitData)
			{
				Init(modelInitData);

				return;
			}

			CFBXRendererAssimp::~CFBXRendererAssimp()
			{
				Terminate();

				return;
			}

			void CFBXRendererAssimp::Terminate()
			{
				Release();

				return;
			}

			void CFBXRendererAssimp::Release()
			{
				m_materialNameTable.clear();
				for (auto& materialDH : m_materialDHs)
				{
					materialDH.second->Release();
				}
				for (auto& diffuseTexture : m_diffuseTextures)
				{
					diffuseTexture.second->Release();
				}
				m_modelDH.Release();
				m_modelCB.Release();
				for (auto* indexBuffer : m_indexBuffers)
				{
					if (indexBuffer)
					{
						indexBuffer->Release();
					}
				}
				for (auto* vertexBuffer : m_vertexBuffers)
				{
					if (vertexBuffer)
					{
						vertexBuffer->Release();
					}
				}
				delete m_importer;
				m_importer = nullptr;
				return;
			}

			bool CFBXRendererAssimp::Init(const SModelInitData& modelInitData)
			{
				auto filePathInChar = modelInitData.modelFilePath;
				auto filePathInWStr = nsUtils::GetWideStringFromString(filePathInChar);
				auto filePathInUTF8 = nsUtils::ToUTF8(filePathInWStr);

				m_importer = new Assimp::Importer;
				int flag = 0;
				flag |= aiProcess_Triangulate;
				flag |= aiProcess_CalcTangentSpace;
				flag |= aiProcess_GenSmoothNormals;
				flag |= aiProcess_GenUVCoords;
				flag |= aiProcess_RemoveRedundantMaterials;
				flag |= aiProcess_OptimizeMeshes;
				flag |= aiProcess_MakeLeftHanded;
				flag |= aiProcess_FlipWindingOrder;
				flag |= aiProcess_FlipUVs;
				flag |= aiProcess_JoinIdenticalVertices;
				flag |= aiProcess_LimitBoneWeights;

				m_scene = m_importer->ReadFile(filePathInUTF8, flag);

				if (m_scene == nullptr)
				{
					nsGameWindow::MessageBoxWarning(L"モデルの読み込みに失敗しました。");
					::OutputDebugStringA(m_importer->GetErrorString());
					::OutputDebugStringA("\n");
					return false;
				}

				const auto& globalTransform = m_scene->mRootNode->mTransformation;
				AiMatrixToMyMatrix(globalTransform, &m_globalInverseTransform);
				m_globalInverseTransform.Inverse();

				InitializeRequiredNodeMap(m_scene->mRootNode);

				auto numMeshes = m_scene->mNumMeshes;


				m_meshes.resize(numMeshes);

				unsigned int NumVertices = 0;
				unsigned int NumIndices = 0;

				for (unsigned int i = 0; i < m_meshes.size(); i++)
				{
					m_meshes[i].MaterialIndex = m_scene->mMeshes[i]->mMaterialIndex;
					m_meshes[i].NumIndices = m_scene->mMeshes[i]->mNumFaces * 3;
					m_meshes[i].BaseVertex = NumVertices;
					m_meshes[i].BaseIndex = NumIndices;

					NumVertices += m_scene->mMeshes[i]->mNumVertices;
					NumIndices += m_meshes[i].NumIndices;
				}
				m_bones.resize(NumVertices);


				std::vector<SMesh> dstMeshes;
				dstMeshes.resize(numMeshes);
				m_materialNameTable.resize(numMeshes);
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* srcMesh = m_scene->mMeshes[meshIdx];
					auto& dstMesh = dstMeshes.at(meshIdx);

					LoadMeshBones(*srcMesh, meshIdx);
					LoadMesh(&dstMesh, *srcMesh, meshIdx);
					const auto& srcMaterial = m_scene->mMaterials[0];
					auto a = m_scene->mNumMaterials;
					LoadTexture(&dstMesh, *srcMaterial, filePathInChar, meshIdx);
				}

				CreateVertexAndIndexBuffer(dstMeshes);

				CreateModelCBV();

				CreateMaterialSRV();

				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				return true;
			}

			void CFBXRendererAssimp::LoadMeshBones(const aiMesh& srcMesh, unsigned int meshIdx)
			{
				for (unsigned int boneIdx = 0; boneIdx < srcMesh.mNumBones; boneIdx++)
				{
					LoadSingleBone(srcMesh.mBones[boneIdx], meshIdx);
				}

				return;

				const unsigned int numBones = srcMesh.mNumBones;
				for (unsigned int boneIdx = 0; boneIdx < numBones; boneIdx++)
				{
					const auto* srcBone = srcMesh.mBones[boneIdx];
					const unsigned int numWeights = srcBone->mNumWeights;
					for (unsigned int weightIdx = 0; weightIdx < numWeights; weightIdx++)
					{
						float weight = srcBone->mWeights[weightIdx].mWeight;
						if (weight < FLT_EPSILON)
						{
							int a = 1;
							continue;
						}
						unsigned int vertId = srcBone->mWeights[weightIdx].mVertexId;
						const auto& boneName = srcBone->mName.data;



						if (m_boneNameAndWeightListTable.count(vertId) > 0)
						{
							auto& boneNameAndWeightListByVertex = 
								m_boneNameAndWeightListTable.at(vertId);
							if (boneNameAndWeightListByVertex.count(boneName) > 0)
							{
								continue;
							}
							else
							{
								boneNameAndWeightListByVertex.emplace(boneName, weight);
							}
						}
						else
						{
							std::unordered_map<std::string, float> boneNameAndWeightListByVertex;
							boneNameAndWeightListByVertex.emplace(boneName, weight);
							m_boneNameAndWeightListTable.emplace(vertId, boneNameAndWeightListByVertex);
						}
						
					}
				}

				return;
			}

			void CFBXRendererAssimp::LoadSingleBone(const aiBone* pBone, unsigned int MeshIndex)
			{
				int BoneId = GetBoneId(pBone);

				if (BoneId == m_boneInfo.size()) 
				{
					nsMath::CMatrix offsetMatrix;
					AiMatrixToMyMatrix(pBone->mOffsetMatrix, &offsetMatrix);
					SBoneInfo bi(offsetMatrix);
					// bi.OffsetMatrix.Print();
					m_boneInfo.push_back(bi);
				}

				for (unsigned int i = 0; i < pBone->mNumWeights; i++) 
				{
					const aiVertexWeight& vw = pBone->mWeights[i];
					unsigned int GlobalVertexID = m_meshes[MeshIndex].BaseVertex + pBone->mWeights[i].mVertexId;
					// printf("%d: %d %f\n",i, pBone->mWeights[i].mVertexId, vw.mWeight);
					m_bones[GlobalVertexID].AddBoneData(BoneId, vw.mWeight);
				}

				MarkRequiredNodesForBone(pBone);
			}

			int CFBXRendererAssimp::GetBoneId(const aiBone* pBone)
			{
				int BoneIndex = 0;
				std::string BoneName(pBone->mName.C_Str());

				if (m_BoneNameToIndexMap.find(BoneName) == m_BoneNameToIndexMap.end()) 
				{
					// Allocate an index for a new bone
					BoneIndex = (int)m_BoneNameToIndexMap.size();
					m_BoneNameToIndexMap[BoneName] = BoneIndex;
				}
				else 
				{
					BoneIndex = m_BoneNameToIndexMap[BoneName];
				}

				return BoneIndex;
			}


			void CFBXRendererAssimp::LoadMesh(
				SMesh* dstMesh, const aiMesh& srcMesh, unsigned int meshIdx)
			{
				aiVector3D zeroVec3D(0.0f, 0.0f, 0.0f);
				aiColor4D zeroColor4D(0.0f, 0.0f, 0.0f, 0.0f);


				// 頂点バッファのコピー

				const unsigned int numVertices = srcMesh.mNumVertices;
				dstMesh->vertices.resize(numVertices);
				for (unsigned int vertIdx = 0; vertIdx < numVertices; vertIdx++)
				{
					const auto& position = srcMesh.mVertices[vertIdx];
					const auto& normal = srcMesh.mNormals[vertIdx];
					const auto& uv = srcMesh.HasTextureCoords(0) ?
						srcMesh.mTextureCoords[0][vertIdx] : zeroVec3D;
					const auto& tangent = srcMesh.HasTangentsAndBitangents() ?
						srcMesh.mTangents[vertIdx] : zeroVec3D;
					const auto& color = srcMesh.HasVertexColors(0) ?
						srcMesh.mColors[0][vertIdx] : zeroColor4D;

					auto& dstVertex = dstMesh->vertices.at(vertIdx);
					dstVertex.position = { position.x, position.y, position.z };
					dstVertex.normal = { normal.x, normal.y, normal.z };
					// @todo タンジェントはそのうち実装
					//dstVertex.tangent = { tangent.x, tangent.y, tangent.z };
					dstVertex.uv = { uv.x, uv.y };
					// uv反転
					//dstVertex.uv.y = 1.0f - dstVertex.uv.y;
					dstVertex.color = { color.r, color.g, color.b, color.a };

					unsigned int globalVertexID = m_meshes[meshIdx].BaseVertex + vertIdx;			
					for (int boneIdx = 0; boneIdx < 4; boneIdx++)
					{
						dstVertex.boneNo[boneIdx] = m_bones[globalVertexID].BoneIDs[boneIdx];
						dstVertex.weights[boneIdx] = 
							static_cast<unsigned short>(m_bones[globalVertexID].Weights[boneIdx] * 10000.0f);
					}
				}


				// インデックスバッファのコピー

				const unsigned int numFaces = srcMesh.mNumFaces;
				dstMesh->indices.resize(numFaces * 3);
				auto& dstIndeices = dstMesh->indices;
				for (unsigned int faceIdx = 0; faceIdx < numFaces; faceIdx++)
				{
					const auto& face = srcMesh.mFaces[faceIdx];
					dstIndeices[faceIdx * 3 + 0] = face.mIndices[0];
					dstIndeices[faceIdx * 3 + 1] = face.mIndices[1];
					dstIndeices[faceIdx * 3 + 2] = face.mIndices[2];

				}


				return;
			}

			void CFBXRendererAssimp::LoadTexture(
				SMesh* dxtMesh,
				const aiMaterial& srcMaterial,
				const char* modelFilePath,
				unsigned int meshIdx
			)
			{
				aiString relativeTexFilePath;
				if (srcMaterial.Get(AI_MATKEY_TEXTURE_DIFFUSE(0), relativeTexFilePath) == AI_SUCCESS)
				{
					auto texFileName = 
						nsUtils::GetFileNameFromFilePath(relativeTexFilePath.C_Str());

					texFileName = "Textures/" + texFileName;

					auto texFilePath = nsUtils::GetTexturePathFromModelAndTexPath(
							modelFilePath, texFileName.c_str());

					dxtMesh->diffuseMapFilePath = texFilePath;

					if (m_diffuseTextures.count(texFilePath) == 0)
					{
						m_diffuseTextures.emplace(texFilePath, new nsDx12Wrappers::CTexture());
						m_diffuseTextures.at(texFilePath)->Init(texFilePath.c_str());
					}
				}
				else
				{
					dxtMesh->diffuseMapFilePath = "";
				}

				m_materialNameTable.at(meshIdx) = dxtMesh->diffuseMapFilePath;

				return;
			}

			bool CFBXRendererAssimp::CreateVertexAndIndexBuffer(const std::vector<SMesh>& meshes)
			{
				static constexpr unsigned int alignSize = 4;
				const auto alignedStrideSize = nsUtils::AlignSize(sizeof(SVertex), alignSize);
				const unsigned int numMeshes = static_cast<unsigned int>(meshes.size());

				// メッシュの数だけ頂点バッファとインデックスバッファを生成
				m_vertexBuffers.resize(numMeshes);
				m_indexBuffers.resize(numMeshes);
				m_numIndicesArray.resize(numMeshes);

				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto& mesh = meshes.at(meshIdx);
					const auto& vertices = mesh.vertices;
					const auto& indices = mesh.indices;
					auto& vertexBuffer = m_vertexBuffers.at(meshIdx);
					auto& indexBuffer = m_indexBuffers.at(meshIdx);
					auto& numIndices = m_numIndicesArray.at(meshIdx);

					vertexBuffer = new nsDx12Wrappers::CVertexBuffer();
					auto resV = vertexBuffer->Init(
						static_cast<unsigned int>(alignedStrideSize * vertices.size()),
						alignedStrideSize,
						&vertices.at(0)
					);

					indexBuffer = new nsDx12Wrappers::CIndexBuffer();
					numIndices = static_cast<unsigned int>(indices.size());
					auto resI = indexBuffer->Init(
						static_cast<unsigned int>(sizeof(indices.at(0)) * indices.size()),
						&meshes.at(meshIdx).indices.at(0)
					);

					if (resV && resI != true)
					{
						return false;
					}
				}

				return true;
			}

			bool CFBXRendererAssimp::CreateModelCBV()
			{
				// 〇定数バッファ作成
				auto cbSize = sizeof(nsMath::CMatrix) * 2;
				m_boneMatrices.resize(256);
				cbSize += sizeof(m_boneMatrices.at(0)) * m_boneMatrices.size();

				//unsigned int boneNum = static_cast<unsigned int>(m_boneMatrixArray.size());
				//cbSize += sizeof(nsMath::CMatrix) * boneNum;

				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"FBXModel");

				nsMath::CMatrix mWorld = nsMath::CMatrix::Identity();

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCB + 2);

				// 〇ディスクリプタヒープ作成
				constexpr unsigned int numDescHeaps = 1;
				m_modelDH.InitAsCbvSrvUav(numDescHeaps, L"FBXModel");

				// 〇定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(m_modelDH.GetCPUHandle());

				return true;
			}

			bool CFBXRendererAssimp::CreateMaterialSRV()
			{
				constexpr unsigned int numDescHeaps = 1;
				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				for (const auto& diffuseTexture : m_diffuseTextures)
				{
					auto* materialDH = new nsDx12Wrappers::CDescriptorHeap();
					materialDH->InitAsCbvSrvUav(numDescHeaps, L"FbxMaterialDH");
					auto matDescHandle = materialDH->GetCPUHandle();

					matSRVDesc.Format = diffuseTexture.second->GetResource()->GetDesc().Format;
					device->CreateShaderResourceView(
						diffuseTexture.second->GetResource(),
						&matSRVDesc,
						matDescHandle
					);

					m_materialDHs.emplace(diffuseTexture.first, materialDH);

				}

				{
					auto* materialDH = new nsDx12Wrappers::CDescriptorHeap();
					materialDH->InitAsCbvSrvUav(numDescHeaps, L"FbxMaterialDH");
					auto matDescHandle = materialDH->GetCPUHandle();

					auto whiteTex = CGraphicsEngine::GetInstance()->GetWhiteTexture();

					matSRVDesc.Format = whiteTex->GetResource()->GetDesc().Format;
					device->CreateShaderResourceView(
						whiteTex->GetResource(),
						&matSRVDesc,
						matDescHandle
					);

					m_materialDHs.emplace("", materialDH);
				}

				return true;
			}


			void CFBXRendererAssimp::InitializeRequiredNodeMap(const aiNode* pNode)
			{
				std::string NodeName(pNode->mName.C_Str());

				SNodeInfo info(pNode);

				m_requiredNodeMap[NodeName] = info;

				for (unsigned int childIdx = 0; childIdx < pNode->mNumChildren; childIdx++)
				{
					InitializeRequiredNodeMap(pNode->mChildren[childIdx]);
				}
				return;
			}

			void CFBXRendererAssimp::MarkRequiredNodesForBone(const aiBone* pBone)
			{
				std::string NodeName(pBone->mName.C_Str());

				const aiNode* pParent = NULL;

				do {
					std::unordered_map<std::string, SNodeInfo>::iterator it = m_requiredNodeMap.find(NodeName);

					if (it == m_requiredNodeMap.end()) 
					{
						printf("Cannot find bone %s in the hierarchy\n", NodeName.c_str());
						assert(0);
					}

					it->second.isRequired = true;

					pParent = it->second.pNode->mParent;

					if (pParent) 
					{
						NodeName = std::string(pParent->mName.C_Str());
					}

				} while (pParent);
			}

			void CFBXRendererAssimp::GetBoneTransforms(
				float TimeInSeconds,
				std::vector<nsMath::CMatrix>* Transforms,
				unsigned int AnimationIndex
			)
			{
				if (AnimationIndex >= m_scene->mNumAnimations)
				{
					printf("Invalid animation index %d, max is %d\n", AnimationIndex, m_scene->mNumAnimations);
					assert(0);
				}

				nsMath::CMatrix Identity;

				float AnimationTimeTicks = CalcAnimationTimeTicks(TimeInSeconds, AnimationIndex);
				const aiAnimation& Animation = *m_scene->mAnimations[AnimationIndex];

				ReadNodeHierarchy(AnimationTimeTicks, m_scene->mRootNode, nsMath::CMatrix::Identity(), Animation);
				Transforms->resize(m_boneInfo.size());

				for (unsigned int boneIdx = 0; boneIdx < m_boneInfo.size(); boneIdx++) 
				{
					(*Transforms)[boneIdx] = m_boneInfo[boneIdx].FinalTransformation;
				}
			}

			float CFBXRendererAssimp::CalcAnimationTimeTicks(
				float TimeInSeconds, unsigned int AnimationIndex)
			{
				float TicksPerSecond = static_cast<float>(
					m_scene->mAnimations[AnimationIndex]->mTicksPerSecond != 0 ? 
					m_scene->mAnimations[AnimationIndex]->mTicksPerSecond : 25.0f
					);
				float TimeInTicks = TimeInSeconds * TicksPerSecond;

				// we need to use the integral part of mDuration for the total length of the animation
				float Duration = 0.0f;
				float fraction = modf(
					static_cast<float>(m_scene->mAnimations[AnimationIndex]->mDuration),
					&Duration
				);

				float AnimationTimeTicks = fmod(TimeInTicks, Duration);
				return AnimationTimeTicks;
			}

			void CFBXRendererAssimp::ReadNodeHierarchy(
				float AnimationTimeTicks,
				const aiNode* pNode,
				const nsMath::CMatrix& ParentTransform,
				const aiAnimation& Animation
			)
			{
				std::string NodeName(pNode->mName.data);

				nsMath::CMatrix NodeTransformation;
				AiMatrixToMyMatrix(pNode->mTransformation, &NodeTransformation);

				const aiNodeAnim* pNodeAnim = FindNodeAnim(Animation, NodeName);

				if (pNodeAnim) 
				{
					SLocalTransform Transform;
					CalcLocalTransform(Transform, AnimationTimeTicks, pNodeAnim);

					nsMath::CMatrix ScalingM;
					ScalingM.MakeScaling(
						Transform.Scaling.x, Transform.Scaling.y, Transform.Scaling.z);
					//        printf("Scaling %f %f %f\n", Transoform.Scaling.x, Transform.Scaling.y, Transform.Scaling.z);

					const auto& rotM3x3 = Transform.Rotation.GetMatrix();
					aiMatrix4x4 rotM4x4(rotM3x3);
					nsMath::CMatrix RotationM;
					AiMatrixToMyMatrix(rotM4x4, &RotationM);

					nsMath::CMatrix TranslationM;
					TranslationM.MakeTranslation(
						Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
					//        printf("Translation %f %f %f\n", Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);

					// Combine the above transformations
					//NodeTransformation = TranslationM * RotationM * ScalingM;
					NodeTransformation = ScalingM * RotationM * TranslationM;
				}

				//nsMath::CMatrix GlobalTransformation = ParentTransform * NodeTransformation;
				nsMath::CMatrix GlobalTransformation = NodeTransformation * ParentTransform;

				if (m_BoneNameToIndexMap.find(NodeName) != m_BoneNameToIndexMap.end()) 
				{
					unsigned int BoneIndex = m_BoneNameToIndexMap[NodeName];
					/*m_boneInfo[BoneIndex].FinalTransformation = 
						m_globalInverseTransform * GlobalTransformation * m_boneInfo[BoneIndex].OffsetMatrix;*/
					m_boneInfo[BoneIndex].FinalTransformation = 
						m_boneInfo[BoneIndex].OffsetMatrix * GlobalTransformation * m_globalInverseTransform;
				}

				for (unsigned int childIdx = 0; childIdx < pNode->mNumChildren; childIdx++) 
				{
					std::string ChildName(pNode->mChildren[childIdx]->mName.data);

					const auto& it = m_requiredNodeMap.find(ChildName);

					if (it == m_requiredNodeMap.end())
					{
						printf("Child %s cannot be found in the required node map\n", ChildName.c_str());
						assert(0);
					}

					if (it->second.isRequired) 
					{
						ReadNodeHierarchy(AnimationTimeTicks, pNode->mChildren[childIdx], GlobalTransformation, Animation);
					}
				}

				return;
			}

			const aiNodeAnim* CFBXRendererAssimp::FindNodeAnim(const aiAnimation&
				Animation, const std::string& NodeName)
			{
				for (unsigned  channelIdx = 0; channelIdx < Animation.mNumChannels; channelIdx++) 
				{
					const aiNodeAnim* pNodeAnim = Animation.mChannels[channelIdx];

					if (std::string(pNodeAnim->mNodeName.data) == NodeName) 
					{
						return pNodeAnim;
					}
				}

				return nullptr;
			}

			void CFBXRendererAssimp::CalcLocalTransform(
				SLocalTransform& Transform,
				float AnimationTimeTicks,
				const aiNodeAnim* pNodeAnim
			)
			{
				CalcInterpolatedScaling(Transform.Scaling, AnimationTimeTicks, pNodeAnim);
				CalcInterpolatedRotation(Transform.Rotation, AnimationTimeTicks, pNodeAnim);
				CalcInterpolatedPosition(Transform.Translation, AnimationTimeTicks, pNodeAnim);
				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedScaling(
				aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				// we need at least two values to interpolate...
				if (pNodeAnim->mNumScalingKeys == 1)
				{
					Out = pNodeAnim->mScalingKeys[0].mValue;
					return;
				}

				unsigned int ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
				unsigned int NextScalingIndex = ScalingIndex + 1;
				assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
				float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
				if (t1 > AnimationTimeTicks)
				{
					Out = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
				}
				else
				{
					float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
					float DeltaTime = t2 - t1;
					float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
					assert(Factor >= 0.0f && Factor <= 1.0f);
					const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
					const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
					aiVector3D Delta = End - Start;
					Out = Start + Factor * Delta;
				}

				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedRotation(
				aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				// we need at least two values to interpolate...
				if (pNodeAnim->mNumRotationKeys == 1) 
				{
					Out = pNodeAnim->mRotationKeys[0].mValue;
					return;
				}

				unsigned int RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
				unsigned int NextRotationIndex = RotationIndex + 1;
				assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
				float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
				if (t1 > AnimationTimeTicks) 
				{
					Out = pNodeAnim->mRotationKeys[RotationIndex].mValue;
				}
				else 
				{
					float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
					float DeltaTime = t2 - t1;
					float Factor = (AnimationTimeTicks - t1) / DeltaTime;
					assert(Factor >= 0.0f && Factor <= 1.0f);
					const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
					const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
					aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
				}

				Out.Normalize();

				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedPosition(
				aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				// we need at least two values to interpolate...
				if (pNodeAnim->mNumPositionKeys == 1)
				{
					Out = pNodeAnim->mPositionKeys[0].mValue;
					return;
				}

				unsigned int PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
				unsigned int NextPositionIndex = PositionIndex + 1;
				assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
				float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
				if (t1 > AnimationTimeTicks) 
				{
					Out = pNodeAnim->mPositionKeys[PositionIndex].mValue;
				}
				else 
				{
					float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
					float DeltaTime = t2 - t1;
					float Factor = (AnimationTimeTicks - t1) / DeltaTime;
					assert(Factor >= 0.0f && Factor <= 1.0f);
					const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
					const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
					aiVector3D Delta = End - Start;
					Out = Start + Factor * Delta;
				}

				return;
			}

			unsigned int CFBXRendererAssimp::FindScaling(
				float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				assert(pNodeAnim->mNumScalingKeys > 0);

				for (unsigned int keyIdx = 0; keyIdx < pNodeAnim->mNumScalingKeys - 1; keyIdx++)
				{
					float t = (float)pNodeAnim->mScalingKeys[keyIdx + 1].mTime;
					if (AnimationTimeTicks < t) 
					{
						return keyIdx;
					}
				}

				return 0;
			}

			unsigned int CFBXRendererAssimp::FindRotation(
				float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				assert(pNodeAnim->mNumRotationKeys > 0);

				for (unsigned int keyIdx = 0; keyIdx < pNodeAnim->mNumRotationKeys - 1; keyIdx++) 
				{
					float t = (float)pNodeAnim->mRotationKeys[keyIdx + 1].mTime;
					if (AnimationTimeTicks < t) 
					{
						return keyIdx;
					}
				}

				return 0;
			}


			unsigned int CFBXRendererAssimp::FindPosition(
				float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
			{
				for (unsigned int keyIdx = 0; keyIdx < pNodeAnim->mNumPositionKeys - 1; keyIdx++)
				{
					float t = (float)pNodeAnim->mPositionKeys[keyIdx + 1].mTime;
					if (AnimationTimeTicks < t) 
					{
						return keyIdx;
					}
				}

				return 0;
			}



		}
	}
}