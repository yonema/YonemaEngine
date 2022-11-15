#include "FBXRendererAssimp.h"
#include "../AssimpCommonHeader.h"
#include "../GraphicsEngine.h"
#include "../Animations/Skelton.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
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
				if (m_skelton)
				{
					delete m_skelton;
				}
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
				if (ImportScene(modelInitData.modelFilePath) != true)
				{
					return false;
				}
				

				m_skelton = new nsAnimations::CSkelton();
				m_skelton->Init(*m_scene->mRootNode);


				auto numMeshes = m_scene->mNumMeshes;


				m_meshInfoArray.resize(numMeshes);

				unsigned int numVertices = 0;
				unsigned int numIndices = 0;

				std::vector<unsigned int> baseVertexNoArray(numMeshes);
				for (unsigned int i = 0; i < numMeshes; i++)
				{
					m_meshInfoArray[i].materialIndex = m_scene->mMeshes[i]->mMaterialIndex;
					m_meshInfoArray[i].numIndices = m_scene->mMeshes[i]->mNumFaces * 3;
					m_meshInfoArray[i].baseVertexNo = numVertices;
					m_meshInfoArray[i].baseIndexNo = numIndices;
					baseVertexNoArray[i] = numVertices;
					numVertices += m_scene->mMeshes[i]->mNumVertices;
					numVertices += m_meshInfoArray[i].numIndices;
				}

				m_skelton->LoadBones(numMeshes, m_scene->mMeshes, baseVertexNoArray, numVertices);


				std::vector<SMesh> dstMeshes;
				dstMeshes.resize(numMeshes);
				m_materialNameTable.resize(numMeshes);
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* srcMesh = m_scene->mMeshes[meshIdx];
					auto& dstMesh = dstMeshes.at(meshIdx);

					LoadMesh(&dstMesh, *srcMesh, meshIdx);
					const auto& srcMaterial = m_scene->mMaterials[0];
					auto a = m_scene->mNumMaterials;
					LoadTexture(&dstMesh, *srcMaterial, modelInitData.modelFilePath, meshIdx);
				}

				CreateVertexAndIndexBuffer(dstMeshes);

				CreateModelCBV();

				CreateMaterialSRV();

				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				return true;
			}

			bool CFBXRendererAssimp::ImportScene(const char* modelFilePath)
			{
				// utf8のファイルパス文字列が必要なため変換。

				auto filePathInChar = modelFilePath;
				auto filePathInWStr = nsUtils::GetWideStringFromString(filePathInChar);
				auto filePathInUTF8Str = nsUtils::ToUTF8(filePathInWStr);

				m_importer = new Assimp::Importer;

				// インポートのポストプロセス設定。
				static constexpr int kPostprocessFlag =
				aiProcess_CalcTangentSpace			|	// 接線と従法線を計算する
				aiProcess_JoinIdenticalVertices		|	// メッシュ内の同一頂点の結合
				aiProcess_MakeLeftHanded			|	// 左手座標系に変換。DirectXの場合必須。
				aiProcess_Triangulate				|	// 全てのポリゴンを三角形ポリゴンに変換
				aiProcess_GenSmoothNormals			|	// 全ての頂点になめらかな法線を生成
				aiProcess_LimitBoneWeights			|	// 1つの頂点に影響を与えるボーンの最大数を制限
				aiProcess_ImproveCacheLocality		|	// 頂点キャッシュの局所性を高めるために三角形の順序を変更
				aiProcess_RemoveRedundantMaterials	|	// 冗長なマテリアルの消去
				aiProcess_FindInvalidData			|	// 無効なデータを探し、消去・修正
				aiProcess_GenUVCoords				|	// UV以外のマッピングをUVへ変換
				aiProcess_OptimizeMeshes			|	// メッシュを最適化して数を減らす
				aiProcess_FlipUVs					|	// UV座標のY座標を反転する。DirectXの場合必須。
				aiProcess_FlipWindingOrder;				// CCWをCWにする。背面を右回りでカリングする。DirectXの場合必須。


				m_scene = m_importer->ReadFile(filePathInUTF8Str, kPostprocessFlag);

				if (m_scene == nullptr)
				{
					std::wstring wstr = filePathInWStr;
					wstr += L"\n上記のモデルの読み込みに失敗しました。";
					nsGameWindow::MessageBoxWarning(wstr.c_str());
					::OutputDebugStringA(m_importer->GetErrorString());
					::OutputDebugStringA("\n");
					return false;
				}

				return true;
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
					dstVertex.color = { color.r, color.g, color.b, color.a };

					unsigned int globalVertexID = m_meshInfoArray[meshIdx].baseVertexNo + vertIdx;
					for (int boneIdx = 0; boneIdx < 4; boneIdx++)
					{
						dstVertex.boneNo[boneIdx] = m_skelton->GetVertexBoneID(globalVertexID, boneIdx);
						dstVertex.weights[boneIdx] = m_skelton->GetVertexWeight(globalVertexID, boneIdx);
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


			void CFBXRendererAssimp::GetBoneTransforms(
				float timeInSeconds,
				std::vector<nsMath::CMatrix>* transforms,
				unsigned int animIdx
			)
			{
				if (animIdx >= m_scene->mNumAnimations)
				{
					// 指定されたアニメーションインデックスが、アニメーションの数を超えてる。
#ifdef _DEBUG
					char buffer[256];
					sprintf_s(buffer, "Invalid animation index %d, max is %d\n", animIdx, m_scene->mNumAnimations);
					::OutputDebugStringA(buffer);
#endif
					return;
				}

				float animTimeTicks = CalcAnimationTimeTicks(timeInSeconds, animIdx);
				const aiAnimation& animation = *m_scene->mAnimations[animIdx];

				ReadNodeHierarchy(
					animTimeTicks, *m_scene->mRootNode, nsMath::CMatrix::Identity(), animation);

				const auto& boneInfoArray = m_skelton->GetBoneInfoArray();
				unsigned int numBoneInfoArray = static_cast<unsigned int>(boneInfoArray.size());
				transforms->resize(numBoneInfoArray);

				for (unsigned int boneIdx = 0; boneIdx < numBoneInfoArray; boneIdx++)
				{
					(*transforms)[boneIdx] = boneInfoArray[boneIdx].mFinalTransform;
				}

				return;
			}

			float CFBXRendererAssimp::CalcAnimationTimeTicks(
				float timeInSeconds, unsigned int animIdx)
			{
				float ticksPerSecond = 
					m_scene->mAnimations[animIdx]->mTicksPerSecond != 0 ?
					static_cast<float>(m_scene->mAnimations[animIdx]->mTicksPerSecond) : 25.0f;
				float timeInTicks = timeInSeconds * ticksPerSecond;

				// we need to use the integral part of mDuration for the total length of the animation
				float duration = 0.0f;
				float fraction = modf(
					static_cast<float>(m_scene->mAnimations[animIdx]->mDuration),
					&duration
				);

				float animTimeTicks = fmod(timeInTicks, duration);
				return animTimeTicks;
			}

			void CFBXRendererAssimp::ReadNodeHierarchy(
				float animTimeTicks,
				const aiNode& node,
				const nsMath::CMatrix& parentTransform,
				const aiAnimation& animation
			)
			{
				nsMath::CMatrix mNodeTransform;
				nsAssimpCommon::AiMatrixToMyMatrix(node.mTransformation, &mNodeTransform);

				std::string nodeName(node.mName.data);
				const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, nodeName);

				if (pNodeAnim) 
				{
					nsAssimpCommon::SLocalTransform localTransform;
					CalcLocalTransform(localTransform, animTimeTicks, *pNodeAnim);

					nsMath::CMatrix mScale, mRot, mTrans;
					mScale.MakeScaling(
						localTransform.scaling.x,
						localTransform.scaling.y,
						localTransform.scaling.z);

					// aiMatrix3x3t から CMatrix の変換は用意してないから、
					// いったん aiMatrix4x4 に変換してから CMatrix に変換する。
					const auto& rotM3x3 = localTransform.rotation.GetMatrix();
					aiMatrix4x4 rotM4x4(rotM3x3);
					nsAssimpCommon::AiMatrixToMyMatrix(rotM4x4, &mRot);

					mTrans.MakeTranslation(
						localTransform.translation.x,
						localTransform.translation.y, 
						localTransform.translation.z);

					// Combine the above transformations
					//NodeTransformation = TranslationM * RotationM * ScalingM;
					mNodeTransform = mScale * mRot * mTrans;
				}

				//nsMath::CMatrix GlobalTransformation = ParentTransform * NodeTransformation;
				nsMath::CMatrix mGlobalTransform = mNodeTransform * parentTransform;

				const auto& boneNameToIndexMap = m_skelton->GetBoneNameToIndexMap();
				if (boneNameToIndexMap.count(nodeName) > 0)
				{
					unsigned int boneIdx = boneNameToIndexMap.at(nodeName);
					/*m_boneInfo[BoneIndex].FinalTransformation = 
						m_globalInverseTransform * GlobalTransformation * m_boneInfo[BoneIndex].OffsetMatrix;*/
					m_skelton->SetBoneFinalTransformMatrix(boneIdx, mGlobalTransform);
				}

				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++) 
				{
					std::string childName(node.mChildren[childIdx]->mName.data);

					const auto& requiredNodeMap = m_skelton->GetRequiredNodeMap();
					const auto& it = requiredNodeMap.find(childName);

					if (it == requiredNodeMap.end())
					{
#ifdef _DEBUG
						char buffer[256];
						sprintf_s(buffer, "Child %s cannot be found in the required node map\n", childName.c_str());
						::OutputDebugStringA(buffer);
#endif
						return;
					}

					if (it->second.isRequired) 
					{
						ReadNodeHierarchy(animTimeTicks, *node.mChildren[childIdx], mGlobalTransform, animation);
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
				nsAssimpCommon::SLocalTransform& localTransform,
				float animTimeTicks,
				const aiNodeAnim& nodeAnim
			)
			{
				CalcInterpolatedScaling(&localTransform.scaling, animTimeTicks, nodeAnim);
				CalcInterpolatedRotation(&localTransform.rotation, animTimeTicks, nodeAnim);
				CalcInterpolatedPosition(&localTransform.translation, animTimeTicks, nodeAnim);
				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedScaling(
				aiVector3D* pScaling, float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumScalingKeys == 1)
				{
					*pScaling = nodeAnim.mScalingKeys[0].mValue;
					return;
				}

				unsigned int scalingIdx = FindScaling(animTimeTicks, nodeAnim);
				unsigned int nextScalingIdx = scalingIdx + 1;
				assert(nextScalingIdx < nodeAnim.mNumScalingKeys);

				float t1 = static_cast<float>(nodeAnim.mScalingKeys[scalingIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pScaling = nodeAnim.mScalingKeys[scalingIdx].mValue;
				}
				else
				{
					float t2 = static_cast<float>(nodeAnim.mScalingKeys[nextScalingIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiVector3D& start = nodeAnim.mScalingKeys[scalingIdx].mValue;
					const aiVector3D& end = nodeAnim.mScalingKeys[nextScalingIdx].mValue;
					aiVector3D delta = end - start;
					*pScaling = start + factor * delta;
				}

				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedRotation(
				aiQuaternion* pRotation, float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumRotationKeys == 1)
				{
					*pRotation = nodeAnim.mRotationKeys[0].mValue;
					return;
				}

				unsigned int rotationIdx = FindRotation(animTimeTicks, nodeAnim);
				unsigned int nextRotationIdx = rotationIdx + 1;
				assert(nextRotationIdx < nodeAnim.mNumRotationKeys);

				float t1 = static_cast<float>(nodeAnim.mRotationKeys[rotationIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pRotation = nodeAnim.mRotationKeys[rotationIdx].mValue;
				}
				else 
				{
					float t2 = static_cast<float>(nodeAnim.mRotationKeys[nextRotationIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiQuaternion& startRotationQ = nodeAnim.mRotationKeys[rotationIdx].mValue;
					const aiQuaternion& endRotationQ = nodeAnim.mRotationKeys[nextRotationIdx].mValue;
					aiQuaternion::Interpolate(*pRotation, startRotationQ, endRotationQ, factor);
				}

				pRotation->Normalize();

				return;
			}

			void CFBXRendererAssimp::CalcInterpolatedPosition(
				aiVector3D* pPosition, float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumPositionKeys == 1)
				{
					*pPosition = nodeAnim.mPositionKeys[0].mValue;
					return;
				}

				unsigned int positionIdx = FindPosition(animTimeTicks, nodeAnim);
				unsigned int nextPositionIdx = positionIdx + 1;
				assert(nextPositionIdx < nodeAnim.mNumPositionKeys);

				float t1 = static_cast<float>(nodeAnim.mPositionKeys[positionIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pPosition = nodeAnim.mPositionKeys[positionIdx].mValue;
				}
				else 
				{
					float t2 = static_cast<float>(nodeAnim.mPositionKeys[nextPositionIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiVector3D& start = nodeAnim.mPositionKeys[positionIdx].mValue;
					const aiVector3D& end = nodeAnim.mPositionKeys[nextPositionIdx].mValue;
					aiVector3D delta = end - start;
					*pPosition = start + factor * delta;
				}

				return;
			}

			unsigned int CFBXRendererAssimp::FindScaling(
				float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				assert(nodeAnim.mNumScalingKeys > 0);

				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumScalingKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mScalingKeys[keyIdx + 1].mTime);
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return 0;
			}

			unsigned int CFBXRendererAssimp::FindRotation(
				float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				assert(nodeAnim.mNumRotationKeys > 0);

				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumRotationKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mRotationKeys[keyIdx + 1].mTime);
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return 0;
			}


			unsigned int CFBXRendererAssimp::FindPosition(
				float animTimeTicks, const aiNodeAnim& nodeAnim)
			{
				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumPositionKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mPositionKeys[keyIdx + 1].mTime);
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return 0;
			}



		}
	}
}