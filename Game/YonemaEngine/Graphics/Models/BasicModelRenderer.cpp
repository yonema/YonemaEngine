#include "BasicModelRenderer.h"
#include "../GraphicsEngine.h"
#include "AssimpCommon.h"
#include "../../Thread/LoadModelThread.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"
#include "../../Memory/ResourceBankTable.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			void CBasicModelRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				if (CheckIsDrawing() != true)
				{
					return;
				}

				commandList->SetDescriptorHeap(m_descriptorHeap);


				// モデルごとの定数バッファのセット
				auto handle = m_descHandlePerModel.GetGpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enModelCBV));
				commandList->SetGraphicsRootDescriptorTable(0, handle);

				// スケルタルアニメーションが有効なら、ボーン行列の配列をセット
				// インスタンシングが有効なら、ワールド行列の配列をセット
				// ボーン行列の配列とワールド行列の配列は、一緒のディスクリプタテーブルにある。
				if (IsSkeltalAnimationValid() || m_modelInitDataRef->maxInstance > 1)
				{
					handle = m_descHandlePerModel.GetGpuHandle(
						static_cast<unsigned int>(EnDescHeapLayoutPerModel::enBoneMatrixArraySRV));
					commandList->SetGraphicsRootDescriptorTable(1, handle);
				}

				// シャドウマップをセット
				if (m_modelInitDataRef->rendererType !=
					nsRenderers::CRendererTable::EnRendererType::enSkyCube)
				{
					handle = m_descHandlePerModel.GetGpuHandle(
						static_cast<unsigned int>(EnDescHeapLayoutPerModel::enShadowMapSRV));
					commandList->SetGraphicsRootDescriptorTable(2, handle);
				}
				
				// マテリアルごとに描画
				// 最初にマテリアルごとにメッシュ分解しているため、メッシュごとに描画と同意。
				const unsigned int numMeshes = static_cast<unsigned int>(m_meshInfoArray.size());
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* indexBuffer = m_indexBuffers.at(meshIdx);
					const auto* vertexBuffer = m_vertexBuffers.at(meshIdx);
					const auto& meshInfo = m_meshInfoArray.at(meshIdx);

					// マテリアルごとの定数バッファをセット;
					auto handleIdx = meshInfo.materialIndex * 
						static_cast<unsigned int>(EnDescHeapLayoutPerMaterial::enNum);
					handle = m_descHandlePerMaterial.GetGpuHandle(handleIdx);
					if (m_modelInitDataRef->rendererType ==
						nsRenderers::CRendererTable::EnRendererType::enSkyCube)
					{
						commandList->SetGraphicsRootDescriptorTable(1, handle);
					}
					else
					{
						commandList->SetGraphicsRootDescriptorTable(3, handle);
					}

					// 頂点バッファとインデックスバッファをセット
					commandList->SetIndexBuffer(*indexBuffer);
					commandList->SetVertexBuffer(*vertexBuffer);

					// 描画
					commandList->DrawInstanced(meshInfo.numIndices, m_fixNumInstanceOnFrame);

				}
				return;
			}

			CBasicModelRenderer::CBasicModelRenderer(
				const nsRenderers::SModelInitData& modelInitData, 
				bool isLODModel,
				std::shared_ptr<nsAnimations::CAnimator>* pAnimator
			)
			{
				m_isLODModel = isLODModel;
				if (pAnimator)
				{
					m_animator = *pAnimator;
				}

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
				m_isImportedModelScene = false;
				m_loadingState = EnLoadingState::enBeforeLoading;

				m_shadowModelRenderer.Release();

				for (auto& geomData : m_geometryDataArray)
				{
					if (geomData)
					{
						delete geomData;
						geomData = nullptr;
					}
				}
				m_geometryDataArray.clear();

				m_boneMatrixArraySB.Release();
				m_worldMatrixArraySB.Release();
				
				for (auto& diffuseTexture : m_diffuseTextures)
				{
					if (diffuseTexture == nullptr)
					{
						continue;
					}

					if (diffuseTexture->IsShared())
					{
						diffuseTexture = nullptr;
						continue;
					}

					diffuseTexture->Release();
					delete diffuseTexture;
					diffuseTexture = nullptr;
				}
				m_diffuseTextures.clear();
				for (auto& normalTexture : m_normalTextures)
				{
					if (normalTexture == nullptr)
					{
						continue;
					}

					if (normalTexture->IsShared())
					{
						normalTexture = nullptr;
						continue;
					}

					normalTexture->Release();
					delete normalTexture;
					normalTexture = nullptr;
				}
				m_normalTextures.clear();
				m_modelCB.Release();
				m_descriptorHeap.Release();
				for (auto* indexBuffer : m_indexBuffers)
				{
					if (indexBuffer)
					{
						indexBuffer->Release();
						delete indexBuffer;
						indexBuffer = nullptr;
					}
				}
				for (auto* vertexBuffer : m_vertexBuffers)
				{
					if (vertexBuffer)
					{
						vertexBuffer->Release();
						delete vertexBuffer;
						vertexBuffer = nullptr;
					}
				}
				if (m_skelton)
				{
					delete m_skelton;
					m_skelton = nullptr;
				}
				if (m_animator)
				{
					m_animator.reset();
				}
				return;
			}

			bool CBasicModelRenderer::Init(const nsRenderers::SModelInitData& modelInitData) noexcept
			{
				m_isImportedModelScene = false;
				m_modelInitDataRef = &modelInitData;

				if (modelInitData.GetFlags(nsRenderers::EnModelInitDataFlags::enLoadingAsynchronous))
				{
					m_loadingState = EnLoadingState::enNowLoading;
					nsThread::CLoadModelThread::GetInstance()->PushLoadModelProcess(
						nsThread::CLoadModelThread::EnLoadProcessType::enLoadModel,
						this,
						&m_animator
					);

					return true;
				}
				else
				{
					m_loadingState = EnLoadingState::enAfterLoading;
					m_isImportedModelScene = true;
				}

				Assimp::Importer* importer = nullptr;
				const aiScene* scene = nullptr;

				const char* modelFilePath = nullptr;
				if (m_isLODModel != true)
				{
					modelFilePath = modelInitData.modelFilePath.c_str();
				}
				else
				{
					modelFilePath = modelInitData.lodMedelFilePath.c_str();
				}

				if (nsAssimpCommon::ImportScene(
					modelFilePath,
					importer,
					scene,
					nsAssimpCommon::g_kBasicRemoveComponentFlags,
					nsAssimpCommon::g_kBasicPostprocessFlags
				) != true)
				{
					return false;
				}

				InitSkeltalAnimation(modelInitData, scene);

				InitAfterImportScene(modelInitData, scene);

				
				
				return true;
			}

			bool CBasicModelRenderer::InitAsynchronous(
				std::shared_ptr<nsAnimations::CAnimator>* pAnimator) noexcept
			{
				m_animator = *pAnimator;
				const char* modelFilePath = nullptr;
				if (m_isLODModel != true)
				{
					modelFilePath = m_modelInitDataRef->modelFilePath.c_str();
				}
				else
				{
					modelFilePath = m_modelInitDataRef->lodMedelFilePath.c_str();
				}

				if (nsAssimpCommon::ImportScene(
					modelFilePath,
					m_importerForLoadAsynchronous,
					m_sceneForLoadAsynchronous,
					nsAssimpCommon::g_kBasicRemoveComponentFlags,
					nsAssimpCommon::g_kBasicPostprocessFlags
				) != true)
				{
					return false;
				}

				InitSkeltalAnimation(*m_modelInitDataRef, m_sceneForLoadAsynchronous);

				m_isImportedModelScene = true;

				return true;
			}

			void CBasicModelRenderer::InitAfterImportScene()
			{
				InitAfterImportScene(*m_modelInitDataRef, m_sceneForLoadAsynchronous);

				return;
			}

			void CBasicModelRenderer::InitAfterImportScene(
				const nsRenderers::SModelInitData& modelInitData,
				const aiScene* scene
			)
			{
				if (scene == nullptr)
				{
					return;
				}

				const auto kNumMeshes = scene->mNumMeshes;
				unsigned int numVertices = 0;
				unsigned int numIndices = 0;
				std::vector<unsigned int> baseVertexNoArray = {};

				if (IsSkeltalAnimationValid())
				{
					InitMeshInfoArray(
						scene, kNumMeshes, &numVertices, &numIndices, &baseVertexNoArray);
					m_skelton->LoadBones(
						kNumMeshes,
						scene->mMeshes,
						baseVertexNoArray,
						numVertices,
						modelInitData.retargetSkeltonName
					);

				}
				else
				{
					InitMeshInfoArray(scene, kNumMeshes, &numVertices, &numIndices);
				}

				std::vector<SMesh> dstMeshes = {};
				LoadMeshes(modelInitData, scene, &dstMeshes, kNumMeshes);
				LoadMaterials(modelInitData, scene);
				CreateVertexAndIndexBuffer(dstMeshes);
				CopyToPhysicsMeshGeometryBuffer(dstMeshes, modelInitData, numVertices, numIndices);
				


				CreateDescriptorHeap();
				CreateModelCBV();
				CreateExpandCBV();
				CreateMaterialSRV();
				CreateBoneMatrisArraySB();
				CreateWorldMatrixArraySB(modelInitData);
				CreateShadowMapSRV();

				m_geometryDataArray.reserve(modelInitData.maxInstance);
				m_geometryDataArray.emplace_back(new nsGeometries::CGeometryData());
				m_geometryDataArray[0]->Init(dstMeshes);
				for (unsigned int instanceIdx = 1; instanceIdx < modelInitData.maxInstance; instanceIdx++)
				{
					m_geometryDataArray.emplace_back(new nsGeometries::CGeometryData());
					m_geometryDataArray[instanceIdx]->Init(m_geometryDataArray[0]->GetAABB());
				}

				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				using EnRendererType = nsRenderers::CRendererTable::EnRendererType;
				using EnModelInitDataFlags = nsRenderers::EnModelInitDataFlags;

				if (modelInitData.rendererType != EnRendererType::enBasicModel)
				{
					// 初期化データにレンダラータイプが設定されていたら、それを優先する。
					SetRenderType(modelInitData.rendererType);
				}
				else if (modelInitData.maxInstance > 2)
				{
					// インスタンシングが有効なら、インスタンシングタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(EnRendererType::enInstancingNonCullingModel);
					}
					else
					{
						SetRenderType(EnRendererType::enInstancingModel);
					}
				}
				else if (IsSkeltalAnimationValid())
				{
					// スキンアニメーションが有効ならスキンタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(EnRendererType::enSkinNonCullingModel);
					}
					else
					{
						SetRenderType(EnRendererType::enSkinModel);
					}
				}
				else
				{
					// その他は基本モデルタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(EnRendererType::enBasicNonCullingModel);
					}
					else
					{
						SetRenderType(EnRendererType::enBasicModel);
					}
				}

				EnableDrawing();



				//if (m_importerForLoadAsynchronous)
				//{
				//	m_importerForLoadAsynchronous->FreeScene();
				//	m_sceneForLoadAsynchronous = nullptr;

				//	delete m_importerForLoadAsynchronous;
				//	m_importerForLoadAsynchronous = nullptr;
				//}

				if (modelInitData.GetFlags(EnModelInitDataFlags::enShadowCaster))
				{
					m_shadowModelRenderer.Init(
						[&](nsDx12Wrappers::CCommandList* commandList)
						{
							DrawShadowModel(commandList);
						},
						GetRenderType(),
						&m_boneMatrices,
						m_modelInitDataRef->maxInstance
					);
				}

				if (m_isLODModel != true)
				{
					SetDrawingFlag(true);
				}

				return;
			}

			bool CBasicModelRenderer::InitSkeltalAnimation(
				const nsRenderers::SModelInitData& modelInitData, const aiScene* scene) noexcept
			{
				bool isSkeltalAnimation = false;

				if (modelInitData.animInitData.numAnimations > 0)
				{
					m_skelton = new nsAnimations::CSkelton();
					m_skelton->Init(*scene->mRootNode);

					if (m_animator == nullptr)
					{
						m_animator = std::make_shared<nsAnimations::CAnimator>();
						isSkeltalAnimation =
							m_animator->Init(
								modelInitData.animInitData,
								modelInitData.GetFlags(EnModelInitDataFlags::enLoadingAsynchronous),
								modelInitData.GetFlags(EnModelInitDataFlags::enRegisterAnimationBank)
							);
					}
					else
					{
						isSkeltalAnimation = true;
					}
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
				const nsRenderers::SModelInitData& modelInitData,
				const aiScene* scene,
				std::vector<SMesh>* destMeshesOut,
				const unsigned int numMeshes
			) noexcept
			{
				auto* node = scene->mRootNode;
				std::list<SMesh> meshesList;
				LoadMeshPerNode(
					modelInitData,
					scene->mRootNode,
					scene,
					nsMath::CMatrix::Identity(),
					&meshesList
				);


				destMeshesOut->reserve(meshesList.size());

				for (const auto& mesh : meshesList)
				{
					SMesh dstMesh = {};
					dstMesh.vertices.reserve(mesh.vertices.size());
					for (const auto& vertex : mesh.vertices)
					{
						dstMesh.vertices.emplace_back(vertex);
					}
					for (const auto& index : mesh.indices)
					{
						dstMesh.indices.emplace_back(index);
					}

					destMeshesOut->emplace_back(dstMesh);
				}

				return;
			}

			void CBasicModelRenderer::LoadMeshPerNode(
				const nsRenderers::SModelInitData& modelInitData,
				aiNode* node,
				const aiScene* scene,
				const nsMath::CMatrix& parentTransform,
				std::list<SMesh>* dstMeshesListOut
			) noexcept
			{
				unsigned int kNumMeshes = node->mNumMeshes;
				nsMath::CMatrix mNodeTransform;
				nsAssimpCommon::AiMatrixToMyMatrix(node->mTransformation, &mNodeTransform);
				nsMath::CMatrix mGlobalTransform = mNodeTransform * parentTransform;
				nsMath::CMatrix mGlobalRotate = mGlobalTransform;
				mGlobalRotate.m_vec4Mat[0].Normalize();
				mGlobalRotate.m_vec4Mat[1].Normalize();
				mGlobalRotate.m_vec4Mat[2].Normalize();
				mGlobalRotate.m_vec4Mat[3] = { 0.0f,0.0f,0.0f,1.0f };

				for (unsigned int meshIdx = 0; meshIdx < kNumMeshes; meshIdx++)
				{
					SMesh dstMesh = {};
					const auto* srcMesh = scene->mMeshes[node->mMeshes[meshIdx]];

					LoadMesh(&dstMesh, *srcMesh, node->mMeshes[meshIdx]);

					if (
						modelInitData.GetFlags(nsRenderers::EnModelInitDataFlags::enNodeTransform)
							== true &&
						IsSkeltalAnimationValid() != true
						)
					{
						for (auto& vertex : dstMesh.vertices)
						{
							mGlobalTransform.Apply(vertex.position);
							mGlobalRotate.Apply(vertex.normal);
							vertex.normal.Normalize();
							dstMesh.mNodeTransformInv = mGlobalTransform;
							dstMesh.mNodeTransformInv.Inverse();
						}
					}

					dstMeshesListOut->emplace_back(dstMesh);
				}

				for (unsigned int childIdx = 0; childIdx < node->mNumChildren; childIdx++)
				{
					LoadMeshPerNode(
						modelInitData,
						node->mChildren[childIdx],
						scene,
						mGlobalTransform,
						dstMeshesListOut
					);
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
					const auto& biNormal = srcMesh.HasTangentsAndBitangents() ?
						srcMesh.mBitangents[vertIdx] : zeroVec3D;
					const auto& color = srcMesh.HasVertexColors(0) ?
						srcMesh.mColors[0][vertIdx] : zeroColor4D;

					// ディスティネーションメッシュにパラメータをコピー

					auto& dstVertex = dstMesh->vertices.at(vertIdx);
					dstVertex.position = { position.x, position.y, position.z };
					dstVertex.normal = { normal.x, normal.y, normal.z };
					// @todo タンジェントはそのうち実装
					dstVertex.tangent = { tangent.x, tangent.y, tangent.z };
					dstVertex.biNormal = { biNormal.x, biNormal.y, biNormal.z };
					dstVertex.uv = { uv.x, uv.y };

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
				m_normalTextures.reserve(kNumMaterials);
				for (unsigned int matIdx = 0; matIdx < kNumMaterials; matIdx++)
				{
					const auto& srcMaterial = scene->mMaterials[matIdx];
					LoadTexture(
						modelInitData,
						*srcMaterial,
						AI_MATKEY_TEXTURE_DIFFUSE(0),
						&m_diffuseTextures
						);

					LoadTexture(
						modelInitData,
						*srcMaterial,
						AI_MATKEY_TEXTURE_NORMALS(0),
						&m_normalTextures
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
				std::vector<nsDx12Wrappers::CTexture*>* texturesOut
			) noexcept
			{
				aiString relativeTexFilePath;
				auto& textureBank = nsMemory::CResourceBankTable::GetInstance()->GetTextureBank();
				const auto& defaultTextures = CGraphicsEngine::GetInstance()->GetDefaultTextures();

				CDefaultTextures::EnTexType texType = CDefaultTextures::EnTexType::enDiffuse;
				if (aiMaterialType == aiTextureType_NORMALS)
				{
					texType = CDefaultTextures::EnTexType::enNormal;
				}

				if (srcMaterial.Get(
					aiMaterialKey, aiMaterialType, aiMaterialIndex, relativeTexFilePath
				) != AI_SUCCESS)
				{
					// テクスチャが設定されていない

					const auto* const filePath = 
						CDefaultTextures::GetTextureFilePath(texType);
					
					auto* texture = textureBank.Get(filePath);
					texturesOut->emplace_back(texture);

					return;
				}

				const auto texFileName =
					nsUtils::GetFileNameFromFilePath(relativeTexFilePath.C_Str());

				if (texFileName == "")
				{
					// テクスチャは設定されているが、名前が設定されていない。
					const auto* const filePath =
						CDefaultTextures::GetTextureFilePath(texType);

					auto* texture = textureBank.Get(filePath);
					texturesOut->emplace_back(texture);

					return;
				}

				auto texFilePath = BuildTextureFilePath(modelInitData, texFileName);

				auto* texture = textureBank.Get(texFilePath.c_str());

				if (texture == nullptr)
				{
					// textureBankに未登録のため、新規作成。

					texture = new nsDx12Wrappers::CTexture();
					if (m_modelInitDataRef->rendererType ==
						nsRenderers::CRendererTable::EnRendererType::enSkyCube)
					{
						texture->InitFromDDSFile(texFilePath.c_str());
					}
					else
					{
						texture->Init(texFilePath.c_str());
					}

					if (modelInitData.GetFlags(
						nsRenderers::EnModelInitDataFlags::enRegisterTextureBank))
					{
						texture->SetShared(true);
						textureBank.Register(texFilePath.c_str(), texture);
					}
				}

				texturesOut->emplace_back(texture);


				return;
			}

			std::string CBasicModelRenderer::BuildTextureFilePath(
				const nsRenderers::SModelInitData& modelInitData,
				const std::string texFileName
			) const noexcept
			{
				std::string texFilePathFromModel = "Textures/";

				if (modelInitData.textureRootPath.empty()!= true)
				{
					texFilePathFromModel += modelInitData.textureRootPath;
					texFilePathFromModel += "/";
				}
				texFilePathFromModel += texFileName;

				const char* modelFilePath = nullptr;
				if (m_isLODModel != true)
				{
					modelFilePath = m_modelInitDataRef->modelFilePath.c_str();
				}
				else
				{
					modelFilePath = m_modelInitDataRef->lodMedelFilePath.c_str();
				}

				return nsUtils::GetTexturePathFromModelAndTexPath(
					modelFilePath, texFilePathFromModel.c_str());
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

			void CBasicModelRenderer::CreateDescriptorHeap()
			{
				constexpr unsigned int kNumDescHeapsPerModel =
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enNum);

				constexpr unsigned int kNumSRVDescHeapsPerMat =
					static_cast<unsigned int>(EnDescHeapLayoutPerMaterial::enNum);
				const unsigned int kNumMaterials =
					static_cast<unsigned int>(m_diffuseTextures.size());
				const unsigned int kNumDescHeapsPerMaterial = kNumSRVDescHeapsPerMat * kNumMaterials;

				const unsigned int kNumDescHeaps =
					kNumDescHeapsPerModel + kNumDescHeapsPerMaterial;

				if (m_modelInitDataRef->rendererType == 
					nsRenderers::CRendererTable::EnRendererType::enSkyCube)
				{
					m_descriptorHeap.InitAsCbvSrvUav(kNumDescHeaps, L"SkyCubeModelRendererDH");
				}
				else
				{
					m_descriptorHeap.InitAsCbvSrvUav(kNumDescHeaps, L"ModelRendererDH");
				}
				auto descHandleCPU = m_descriptorHeap.GetCPUHandle();
				auto descHandleGPU = m_descriptorHeap.GetGPUHandle();

				m_descHandlePerModel.Init(kNumDescHeapsPerModel, descHandleCPU, descHandleGPU);

				const unsigned int lastIndex =
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enNum) - 1;
				auto inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav();
				descHandleCPU = m_descHandlePerModel.GetCpuHandle(lastIndex);
				descHandleGPU = m_descHandlePerModel.GetGpuHandle(lastIndex);
				descHandleCPU.ptr += inc;
				descHandleGPU.ptr += inc;

				m_descHandlePerMaterial.Init(kNumDescHeapsPerMaterial, descHandleCPU, descHandleGPU);


				return;
			}


			bool CBasicModelRenderer::CreateModelCBV()
			{
				// 定数バッファ作成

				// ワールド行列 + ワールドビュープロジェクション行列 + 
				// ライトビュープロジェクション行列 = 3
				auto cbSize = sizeof(SConstantBufferData);

				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"ModelCB");

				const auto& mWorld = nsMath::CMatrix::Identity();
				const auto& mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				auto* mappedCB =
					static_cast<SConstantBufferData*>(m_modelCB.GetMappedConstantBuffer());
				auto* shadowCamera =
					CGraphicsEngine::GetInstance()->GetShadowMapRenderer()->GetCamera();
				const auto& mLightViewProj = shadowCamera->GetViewProjectionMatirx();
				const auto& lightPos = shadowCamera->GetPosition();
				mappedCB->mWorld = mWorld;
				mappedCB->mViewProj = mViewProj;
				mappedCB->mLightViewProj = mLightViewProj;
				mappedCB->lightPos = lightPos;
				mappedCB->isShadowReceiver = 
					m_modelInitDataRef->GetFlags(nsRenderers::EnModelInitDataFlags::enShadowReceiver);

				// 定数バッファビュー作成
				auto handle = m_descHandlePerModel.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enModelCBV));
				m_modelCB.CreateConstantBufferView(handle);

				return true;
			}

			bool CBasicModelRenderer::CreateExpandCBV()
			{
				if (m_modelInitDataRef->pExpandConstantBuffer == nullptr)
				{
					return true;
				}

				// 定数バッファビュー作成
				auto handle = m_descHandlePerModel.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enExpandCBV));
				m_modelInitDataRef->pExpandConstantBuffer->CreateConstantBufferView(handle);


				return true;
			}



			bool CBasicModelRenderer::CreateMaterialSRV()
			{
				auto* device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;
				matSRVDesc.TextureCube.MipLevels = 1;

				unsigned int kNumTextures = static_cast<unsigned int>(m_diffuseTextures.size());
				unsigned int handleIdx = 0;

				for (unsigned int texIdx = 0; texIdx < kNumTextures; texIdx++)
				{
					const auto& diffuseTexDesc = m_diffuseTextures[texIdx]->GetResource()->GetDesc();
					
					matSRVDesc.Format = diffuseTexDesc.Format;
					if (m_diffuseTextures[texIdx]->IsCubemap())
					{
						matSRVDesc.TextureCube.MipLevels = diffuseTexDesc.MipLevels;
						matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					}
					else
					{
						matSRVDesc.Texture2D.MipLevels = diffuseTexDesc.MipLevels;
						matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					}
					device->CreateShaderResourceView(
						m_diffuseTextures[texIdx]->GetResource(),
						&matSRVDesc,
						m_descHandlePerMaterial.GetCpuHandle(handleIdx)
					);

					handleIdx++;

					const auto& normalTexDesc = m_normalTextures[texIdx]->GetResource()->GetDesc();
					matSRVDesc.Format = normalTexDesc.Format;
					if (m_normalTextures[texIdx]->IsCubemap())
					{
						matSRVDesc.TextureCube.MipLevels = normalTexDesc.MipLevels;
						matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					}
					else
					{
						matSRVDesc.Texture2D.MipLevels = normalTexDesc.MipLevels;
						matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					}
					device->CreateShaderResourceView(
						m_normalTextures[texIdx]->GetResource(),
						&matSRVDesc,
						m_descHandlePerMaterial.GetCpuHandle(handleIdx)
					);

					handleIdx++;

				}

				return true;
			}

			bool CBasicModelRenderer::CreateBoneMatrisArraySB()
			{
				unsigned int size = 1;
				unsigned int num = 1;

				if (IsSkeltalAnimationValid())
				{
					size = static_cast<unsigned int>(sizeof(nsMath::CMatrix));
					const auto& boneInfoArray = m_skelton->GetBoneInfoArray();
					num = static_cast<unsigned int>(boneInfoArray.size());
					m_boneMatrices.resize(num);
				}

				bool res =
					m_boneMatrixArraySB.Init(size, num);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_boneMatrixArraySBの生成に失敗しました。");
					return false;
				}

				auto handle = m_descHandlePerModel.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enBoneMatrixArraySRV));
				m_boneMatrixArraySB.RegistShaderResourceView(handle);

				return true;
			}


			bool CBasicModelRenderer::CreateWorldMatrixArraySB(
				const nsRenderers::SModelInitData& modelInitData)
			{
				unsigned int size = 1;
				unsigned int num = 1;

				if (modelInitData.maxInstance > 1)
				{
					size = static_cast<unsigned int>(sizeof(nsMath::CMatrix));
					num = modelInitData.maxInstance;
				}

				bool res = 
					m_worldMatrixArraySB.Init(size, num);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_worldMatrixArraySBの生成に失敗しました。");
					return false;
				}

				auto handle = m_descHandlePerModel.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enWorldMatrixArraySRV));
				m_worldMatrixArraySB.RegistShaderResourceView(handle);

				return true;
			}

			bool CBasicModelRenderer::CreateShadowMapSRV()
			{
				auto* device = CGraphicsEngine::GetInstance()->GetDevice();
				auto* shadowMapTexture =
					CGraphicsEngine::GetInstance()->GetShadowMapRenderer()->GetShadowBokeTexture();

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;
				matSRVDesc.Format = shadowMapTexture->GetResource()->GetDesc().Format;

				auto handle = m_descHandlePerModel.GetCpuHandle(
					static_cast<unsigned int>(EnDescHeapLayoutPerModel::enShadowMapSRV));
				device->CreateShaderResourceView(
					shadowMapTexture->GetResource(),
					&matSRVDesc,
					handle
				);

				return true;
			}



			void CBasicModelRenderer::UpdateWorldMatrix(
				const nsMath::CVector3& position,
				const nsMath::CQuaternion& rotation,
				const nsMath::CVector3& scale
			) noexcept
			{
				if (IsDrawingFlag() != true)
				{
					return;
				}

				// ワールド行列作成。
				nsMath::CMatrix mTrans, mRot, mScale;
				mTrans.MakeTranslation(position);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				m_worldMatrix = m_bias * mScale * mRot * mTrans;

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<SConstantBufferData*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB->mWorld = m_worldMatrix;
				const auto& mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB->mViewProj = mViewProj;
				auto* shadowCamera = 
					CGraphicsEngine::GetInstance()->GetShadowMapRenderer()->GetCamera();
				const auto& mLightViewProj = shadowCamera->GetViewProjectionMatirx();
				mappedCB->mLightViewProj = mLightViewProj;
				mappedCB->lightPos = shadowCamera->GetPosition();


				m_fixNumInstanceOnFrame = 0;

				// ジオメトリデータを更新
				m_geometryDataArray[0]->Update(m_worldMatrix);
				if (m_geometryDataArray[0]->IsInViewFrustum())
				{
					m_fixNumInstanceOnFrame++;
				}

				if (m_shadowModelRenderer.IsValid())
				{
					m_shadowModelRenderer.Update(m_worldMatrix);
				}

				return;
			}

			void CBasicModelRenderer::UpdateAnimation(float deltaTime, bool updateAnimMatrix) noexcept
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return;
				}

				m_animator->UpdateAnimation(deltaTime);

				if (m_geometryDataArray[0]->IsInViewFrustum() && 
					updateAnimMatrix &&
					IsDrawingFlag())
				{
					m_animator->CalcAndGetAnimatedBoneTransforms(&m_boneMatrices, m_skelton);
					m_boneMatrixArraySB.CopyToMappedStructuredBuffer(m_boneMatrices.data());

					m_shadowModelRenderer.UpdateBoneMatrixArray(&m_boneMatrices);
				}


				return;
			}

			unsigned int CBasicModelRenderer::FindBoneId(const std::string& boneName) const noexcept
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return nsAnimations::CSkelton::m_kNotFoundBoneID;
				}

				// ボーン名からボーンIDを検索
				const auto& boneNameToIndexMap = m_skelton->GetBoneNameToIndexMap();
				const auto& boneNameToIdx = boneNameToIndexMap.find(boneName);

				if (boneNameToIdx == boneNameToIndexMap.end())
				{
					// 指定された名前のボーンが見つからなかった
					return nsAnimations::CSkelton::m_kNotFoundBoneID;
				}

				return boneNameToIdx->second;
			}

			void CBasicModelRenderer::CheckLoaded() noexcept
			{
				if (m_loadingState != EnLoadingState::enNowLoading)
				{
					return;
				}

				if (m_isImportedModelScene != true)
				{
					return;
				}

				if (m_animator)
				{
					if (m_animator->IsLoaded() != true)
					{
						return;
					}
				}

				m_loadingState = EnLoadingState::enAfterLoading;


				return;
			}

			void CBasicModelRenderer::UpdateWorldMatrixArray(
				const std::vector<nsMath::CMatrix>& worldMatrixArray)
			{
				if (m_modelInitDataRef->maxInstance > 1 != true || IsDrawingFlag() != true)
				{
					return;
				}

				m_fixNumInstanceOnFrame = 0;

				std::vector<nsMath::CMatrix> fixWorldMatrixArray = {};
				fixWorldMatrixArray.reserve(worldMatrixArray.size());

				auto geomData = m_geometryDataArray.begin();
				for (const auto& mWorld : worldMatrixArray)
				{
					(*geomData)->Update(m_bias * mWorld);
					if ((*geomData)->IsInViewFrustum())
					{
						fixWorldMatrixArray.emplace_back(mWorld);
						m_fixNumInstanceOnFrame++;
					}
					geomData++;
				}

				m_worldMatrixArraySB.CopyToMappedStructuredBuffer(
					fixWorldMatrixArray.data(), 
					sizeof(nsMath::CMatrix) * m_fixNumInstanceOnFrame);

				m_shadowModelRenderer.UpdateWorldMatrixArray(
					&fixWorldMatrixArray, m_fixNumInstanceOnFrame);

				return;
			}


			void CBasicModelRenderer::DrawShadowModel(nsDx12Wrappers::CCommandList* commandList)
			{
				if (CheckIsDrawing() != true)
				{
					return;
				}

				// マテリアルごとに描画
				// 最初にマテリアルごとにメッシュ分解しているため、メッシュごとに描画と同意。
				const unsigned int numMeshes = static_cast<unsigned int>(m_meshInfoArray.size());
				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					const auto* indexBuffer = m_indexBuffers.at(meshIdx);
					const auto* vertexBuffer = m_vertexBuffers.at(meshIdx);
					const auto& meshInfo = m_meshInfoArray.at(meshIdx);

					// 頂点バッファとインデックスバッファをセット
					commandList->SetIndexBuffer(*indexBuffer);
					commandList->SetVertexBuffer(*vertexBuffer);

					// 描画
					commandList->DrawInstanced(meshInfo.numIndices, m_fixNumInstanceOnFrame);

				}

				return;
			}



		}
	}
}