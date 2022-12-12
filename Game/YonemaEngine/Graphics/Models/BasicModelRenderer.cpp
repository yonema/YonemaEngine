#include "BasicModelRenderer.h"
#include "../GraphicsEngine.h"
#include "AssimpCommon.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			void CBasicModelRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				// モデルごとの定数バッファのセット
				ID3D12DescriptorHeap* modelDescHeaps[] = { m_modelDH.Get() };
				commandList->SetDescriptorHeaps(1, modelDescHeaps);
				auto descriptorHeapH = m_modelDH.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);


				// マテリアルごとに描画
				// 最初にマテリアルごとにメッシュ分解しているため、メッシュごとに描画と同意。
				const unsigned int numMeshes = static_cast<unsigned int>(m_meshInfoArray.size());
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* indexBuffer = m_indexBuffers.at(meshIdx);
					const auto* vertexBuffer = m_vertexBuffers.at(meshIdx);
					const auto& meshInfo = m_meshInfoArray.at(meshIdx);

					// マテリアルごとの定数バッファをセット
					const auto* materialDH = m_materialDHs.at(meshInfo.materialIndex);
					ID3D12DescriptorHeap* materialDescHeaps[] = { materialDH->Get() };
					commandList->SetDescriptorHeaps(1, materialDescHeaps);
					descriptorHeapH = materialDH->GetGPUHandle();
					commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);

					// 頂点バッファとインデックスバッファをセット
					commandList->SetIndexBuffer(*indexBuffer);
					commandList->SetVertexBuffer(*vertexBuffer);

					// 描画
					commandList->DrawInstanced(meshInfo.numIndices);

				}
				return;
			}

			CBasicModelRenderer::CBasicModelRenderer(const nsRenderers::SModelInitData& modelInitData)
			{
				Init(modelInitData);

				return;
			}

			CBasicModelRenderer::~CBasicModelRenderer()
			{
				Terminate();

				return;
			}

			void CBasicModelRenderer::Terminate() noexcept
			{
				Release();

				return;
			}

			void CBasicModelRenderer::Release()
			{
				for (auto& materialDH : m_materialDHs)
				{
					if (materialDH)
					{
						materialDH->Release();
					}
				}
				for (auto& diffuseTexture : m_diffuseTextures)
				{
					if (diffuseTexture)
					{
						diffuseTexture->Release();
					}
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
				if (m_skelton)
				{
					delete m_skelton;
				}
				if (m_animator)
				{
					delete m_animator;
				}
				return;
			}

			bool CBasicModelRenderer::Init(const nsRenderers::SModelInitData& modelInitData) noexcept
			{
				Assimp::Importer* importer = nullptr;
				const aiScene* scene = nullptr;

				if (nsAssimpCommon::ImportScene(
					modelInitData.modelFilePath,
					importer,
					scene,
					nsAssimpCommon::g_kRemoveComponentFlags,
					nsAssimpCommon::g_kBasicPostprocessFlags
				) != true)
				{
					return false;
				}

				InitSkeltalAnimation(modelInitData, scene);

				const auto kNumMeshes = scene->mNumMeshes;
				unsigned int numVertices = 0;
				unsigned int numIndices = 0;
				std::vector<unsigned int> baseVertexNoArray = {};

				if (IsSkeltalAnimationValid())
				{
					InitMeshInfoArray(
						scene, kNumMeshes, &numVertices, &numIndices, &baseVertexNoArray);
					m_skelton->LoadBones(
						kNumMeshes, scene->mMeshes, baseVertexNoArray, numVertices);
				}
				else
				{
					InitMeshInfoArray(scene, kNumMeshes, &numVertices, &numIndices);
				}

				std::vector<SMesh> dstMeshes = {};
				LoadMeshes(scene, &dstMeshes, kNumMeshes);
				LoadMaterials(modelInitData, scene);
				CreateVertexAndIndexBuffer(dstMeshes);
				CopyToPhysicsMeshGeometryBuffer(dstMeshes, modelInitData, numVertices, numIndices);
				CreateModelCBV();
				CreateMaterialSRV();
				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				if (IsSkeltalAnimationValid() && 
					modelInitData.rendererType == nsRenderers::CRendererTable::EnRendererType::enBasicModel)
				{
					SetRenderType(nsRenderers::CRendererTable::EnRendererType::enSkinModel);
				}
				else
				{
					SetRenderType(modelInitData.rendererType);
				}
				EnableDrawing();
				
				return true;
			}

			bool CBasicModelRenderer::InitSkeltalAnimation(
				const nsRenderers::SModelInitData& modelInitData, const aiScene* scene) noexcept
			{
				bool isSkeltalAnimation = false;

				if (modelInitData.animInitData)
				{
					m_skelton = new nsAnimations::CSkelton();
					m_skelton->Init(*scene->mRootNode);
					m_animator = new nsAnimations::CAnimator();
					isSkeltalAnimation = 
						m_animator->Init(*modelInitData.animInitData, m_skelton);
				}

				return isSkeltalAnimation;
			}

			void CBasicModelRenderer::InitMeshInfoArray(
				const aiScene* scene,
				const unsigned int numMeshes,
				unsigned int* numVerticesOut,
				unsigned int* numIndicesOut,
				std::vector<unsigned int>* baseVertexNoArrayOut
			) noexcept
			{
				if (baseVertexNoArrayOut)
				{
					// ボーン構築用
					baseVertexNoArrayOut->reserve(numMeshes);
				}

				m_meshInfoArray.reserve(numMeshes);

				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					m_meshInfoArray.emplace_back(
						scene->mMeshes[meshIdx]->mNumFaces * 3,	/* numIndices */
						*numVerticesOut,						/* baseVertexNo */
						*numIndicesOut,							/* baseIndexNo */
						scene->mMeshes[meshIdx]->mMaterialIndex	/* materialIndex */
					);

					if (baseVertexNoArrayOut)
					{
						// ボーン構築用
						baseVertexNoArrayOut->emplace_back(*numVerticesOut);
					}

					*numVerticesOut += scene->mMeshes[meshIdx]->mNumVertices;
					*numIndicesOut += m_meshInfoArray[meshIdx].numIndices;
				}

				return;
			}

			void CBasicModelRenderer::LoadMeshes(
				const aiScene* scene,
				std::vector<SMesh>* destMeshesOut,
				const unsigned int numMeshes
			) noexcept
			{
				destMeshesOut->reserve(numMeshes);
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					SMesh dstMesh = {};
					const auto* srcMesh = scene->mMeshes[meshIdx];

					LoadMesh(&dstMesh, *srcMesh, meshIdx);

					destMeshesOut->emplace_back(dstMesh);
				}

				return;
			}

			void CBasicModelRenderer::LoadMesh(
				SMesh* dstMesh, const aiMesh& srcMesh, unsigned int meshIdx) noexcept
			{
				// パラメータがなかった時用のゼロベクトル
				aiVector3D zeroVec3D(0.0f, 0.0f, 0.0f);
				// パラメータがなかった時用のゼロカラー
				aiColor4D zeroColor4D(0.0f, 0.0f, 0.0f, 0.0f);


				// 頂点バッファのコピー

				const unsigned int kNumVertices = srcMesh.mNumVertices;
				dstMesh->vertices.resize(kNumVertices);
				for (unsigned int vertIdx = 0; vertIdx < kNumVertices; vertIdx++)
				{
					// ソースメッシュからパラメータを取り出す

					const auto& position = srcMesh.mVertices[vertIdx];
					const auto& normal = srcMesh.mNormals[vertIdx];
					const auto& uv = srcMesh.HasTextureCoords(0) ?
						srcMesh.mTextureCoords[0][vertIdx] : zeroVec3D;
					const auto& tangent = srcMesh.HasTangentsAndBitangents() ?
						srcMesh.mTangents[vertIdx] : zeroVec3D;
					const auto& color = srcMesh.HasVertexColors(0) ?
						srcMesh.mColors[0][vertIdx] : zeroColor4D;

					// ディスティネーションメッシュにパラメータをコピー

					auto& dstVertex = dstMesh->vertices.at(vertIdx);
					dstVertex.position = { position.x, position.y, position.z };
					dstVertex.normal = { normal.x, normal.y, normal.z };
					// @todo タンジェントはそのうち実装
					//dstVertex.tangent = { tangent.x, tangent.y, tangent.z };
					dstVertex.uv = { uv.x, uv.y };
					dstVertex.color = { color.r, color.g, color.b, color.a };

					if (IsSkeltalAnimationValid() != true)
					{
						continue;
					}


					unsigned int globalVertexID = m_meshInfoArray[meshIdx].baseVertexNo + vertIdx;
					for (unsigned int boneIdx = 0; boneIdx < m_kMaxNumBoneWeights; boneIdx++)
					{
						dstVertex.boneNo[boneIdx] = 
							m_skelton->GetVertexBoneID(globalVertexID, boneIdx);
						dstVertex.weights[boneIdx] = 
							m_skelton->GetVertexWeight(globalVertexID, boneIdx);
					}

				}


				// インデックスバッファのコピー

				const unsigned int kNumFaces = srcMesh.mNumFaces;
				dstMesh->indices.resize(kNumFaces * 3);
				auto& dstIndeices = dstMesh->indices;
				for (unsigned int faceIdx = 0; faceIdx < kNumFaces; faceIdx++)
				{
					const auto& face = srcMesh.mFaces[faceIdx];
					dstIndeices[faceIdx * 3 + 0] = face.mIndices[0];
					dstIndeices[faceIdx * 3 + 1] = face.mIndices[1];
					dstIndeices[faceIdx * 3 + 2] = face.mIndices[2];

				}


				return;
			}

			void CBasicModelRenderer::LoadMaterials(
				const nsRenderers::SModelInitData& modelInitData, const aiScene* scene) noexcept
			{
				const unsigned int kNumMaterials = scene->mNumMaterials;
				m_diffuseTextures.reserve(kNumMaterials);
				for (unsigned int matIdx = 0; matIdx < kNumMaterials; matIdx++)
				{
					const auto& srcMaterial = scene->mMaterials[matIdx];
					LoadTexture(
						modelInitData,
						*srcMaterial,
						AI_MATKEY_TEXTURE_DIFFUSE(0),
						&m_diffuseTextures,
						matIdx
						);
				}

				return;
			}

			void CBasicModelRenderer::LoadTexture(
				const nsRenderers::SModelInitData& modelInitData,
				const aiMaterial& srcMaterial,
				const char* aiMaterialKey,
				unsigned int aiMaterialType,
				unsigned int aiMaterialIndex,
				std::vector<nsDx12Wrappers::CTexture*>* texturesOut,
				unsigned int matIdx
			) noexcept
			{
				texturesOut->emplace_back(new nsDx12Wrappers::CTexture());
				auto* texture = texturesOut->at(matIdx);

				aiString relativeTexFilePath;
				if (srcMaterial.Get(
					aiMaterialKey, aiMaterialType, aiMaterialIndex, relativeTexFilePath
				) != AI_SUCCESS)
				{
					// テクスチャが設定されていない
					texture->InitFromTexture(
						CGraphicsEngine::GetInstance()->GetWhiteTexture());

					return;
				}

				const auto texFileName =
					nsUtils::GetFileNameFromFilePath(relativeTexFilePath.C_Str());

				if (texFileName == "")
				{
					// テクスチャは設定されているが、名前が設定されていない。
					texture->InitFromTexture(
						CGraphicsEngine::GetInstance()->GetWhiteTexture());
					return;
				}

				auto texFilePath = BuildTextureFilePath(modelInitData, texFileName);


				// テクスチャの初期化
				texture->Init(texFilePath.c_str());

				return;
			}

			std::string CBasicModelRenderer::BuildTextureFilePath(
				const nsRenderers::SModelInitData& modelInitData,
				const std::string texFileName
			) const noexcept
			{
				std::string texFilePathFromModel = "Textures/";

				if (modelInitData.textureRootPath)
				{
					texFilePathFromModel += modelInitData.textureRootPath;
					texFilePathFromModel += "/";
				}
				texFilePathFromModel += texFileName;

				return nsUtils::GetTexturePathFromModelAndTexPath(
					modelInitData.modelFilePath, texFilePathFromModel.c_str());
			}


			bool CBasicModelRenderer::CreateVertexAndIndexBuffer(const std::vector<SMesh>& meshes) noexcept
			{
				const auto alignedStrideSize = 
					nsUtils::AlignSize(sizeof(SVertex), m_kAligSizeVertexStride);
				const unsigned int kNumMeshes = static_cast<unsigned int>(meshes.size());

				// メッシュの数だけ頂点バッファとインデックスバッファを生成
				m_vertexBuffers.reserve(kNumMeshes);
				m_indexBuffers.reserve(kNumMeshes);

				for (unsigned int meshIdx = 0; meshIdx < kNumMeshes; meshIdx++)
				{
					const auto& mesh = meshes.at(meshIdx);
					const auto& vertices = mesh.vertices;
					const auto& indices = mesh.indices;

					m_vertexBuffers.emplace_back(new nsDx12Wrappers::CVertexBuffer());
					const bool resV = m_vertexBuffers.at(meshIdx)->Init(
						static_cast<unsigned int>(alignedStrideSize * vertices.size()),
						alignedStrideSize,
						&vertices.at(0)
					);

					m_indexBuffers.emplace_back(new nsDx12Wrappers::CIndexBuffer());
					const bool resI = m_indexBuffers.at(meshIdx)->Init(
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

			void CBasicModelRenderer::CopyToPhysicsMeshGeometryBuffer(
				const std::vector<SMesh>& meshes,
				const nsRenderers::SModelInitData& modelInitData,
				unsigned int numVertices,
				unsigned int numIndices
			)
			{
				if (modelInitData.physicsMeshGeomBuffer == nullptr)
				{
					return;
				}
				auto& bufferVertices = modelInitData.physicsMeshGeomBuffer->m_vertices;
				auto& bufferIndices = modelInitData.physicsMeshGeomBuffer->m_indices;

				// メッシュの必要な情報を、バッファーにコピー。
				bufferVertices.reserve(numVertices);
				bufferIndices.reserve(numIndices);

				const unsigned int kNumMeshes = static_cast<unsigned int>(meshes.size());
				for (unsigned int meshIdx = 0; meshIdx < kNumMeshes; meshIdx++)
				{
					const auto& mesh = meshes.at(meshIdx);

					for (const auto& vertex : mesh.vertices)
					{
						bufferVertices.emplace_back(vertex.position);
					}
					for (const auto& index : mesh.indices)
					{
						bufferIndices.emplace_back(index);
					}

				}

				// モデルのバイアスに合わせて、物理メッシュ用の頂点も回しておく。
				modelInitData.physicsMeshGeomBuffer->RotateVertices(modelInitData.vertexBias);

				return;
			}

			bool CBasicModelRenderer::CreateModelCBV()
			{
				// 定数バッファ作成

				// ワールド行列 + ワールドビュープロジェクション行列 = 2
				auto cbSize = sizeof(nsMath::CMatrix) * 2;
				if (IsSkeltalAnimationValid())
				{
					m_boneMatrices.resize(m_kMaxNumBones);
					cbSize += sizeof(m_boneMatrices.at(0)) * m_boneMatrices.size();
				}

				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"ModelCB");

				const auto& mWorld = nsMath::CMatrix::Identity();
				const auto& mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				auto* mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				mappedCB[1] = mWorld * mViewProj;

				if (IsSkeltalAnimationValid())
				{
					copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCB + 2);
				}

				// ディスクリプタヒープ作成
				constexpr unsigned int kNumDescHeaps = 1;
				m_modelDH.InitAsCbvSrvUav(kNumDescHeaps, L"ModelCB");

				// 定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(m_modelDH.GetCPUHandle());

				return true;
			}


			bool CBasicModelRenderer::CreateMaterialSRV()
			{
				constexpr unsigned int numDescHeaps = 1;
				auto* device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				m_materialDHs.reserve(m_diffuseTextures.size());
				for (const auto& diffuseTexture : m_diffuseTextures)
				{
					auto* materialDH = new nsDx12Wrappers::CDescriptorHeap();
					materialDH->InitAsCbvSrvUav(numDescHeaps, L"MaterialDH");
					auto matDescHandle = materialDH->GetCPUHandle();

					matSRVDesc.Format = diffuseTexture->GetResource()->GetDesc().Format;
					device->CreateShaderResourceView(
						diffuseTexture->GetResource(),
						&matSRVDesc,
						matDescHandle
					);

					m_materialDHs.emplace_back(materialDH);
				}

				return true;
			}


			void CBasicModelRenderer::UpdateWorldMatrix(
				const nsMath::CVector3& position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale
			) noexcept
			{
				// ワールド行列作成。
				nsMath::CMatrix mTrans, mRot, mScale;
				mTrans.MakeTranslation(position);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				m_worldMatrix = m_bias * mScale * mRot * mTrans;

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = m_worldMatrix;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = m_worldMatrix * mViewProj;

				return;
			}

			void CBasicModelRenderer::UpdateAnimation(float deltaTime) noexcept
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return;
				}

				m_animator->UpdateAnimation(deltaTime);

				m_animator->CalcAndGetAnimatedBoneTransforms(&m_boneMatrices);

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());

				copy(m_boneMatrices.begin(), m_boneMatrices.end(), mappedCB + 2);

				return;
			}

			unsigned int CBasicModelRenderer::FindBoneId(const std::string& boneName) const noexcept
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return UINT_MAX;
				}

				// ボーン名からボーンIDを検索
				const auto& boneNameToIndexMap = m_skelton->GetBoneNameToIndexMap();
				const auto& boneNameToIdx = boneNameToIndexMap.find(boneName);

				if (boneNameToIdx == boneNameToIndexMap.end())
				{
					// 指定された名前のボーンが見つからなかった
					return UINT_MAX;
				}

				return boneNameToIdx->second;
			}


		}
	}
}