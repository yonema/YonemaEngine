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
				if (m_loadingState != EnLoadingState::enAfterLoading)
				{
					// ロード中は描画できない
					return;
				}

				if (m_fixNumInstanceOnFrame <= 0)
				{
					// 描画するインスタンスがない
					return;
				}

				// モデルごとの定数バッファのセット
				{
					ID3D12DescriptorHeap* descHeaps[] = { m_modelDH.Get() };
					commandList->SetDescriptorHeaps(1, descHeaps);
				}
				auto descriptorHeapH = m_modelDH.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);

				// スケルたタルアニメーションが有効なら、ボーン行列の配列をセット
				if (IsSkeltalAnimationValid())
				{
					ID3D12DescriptorHeap* descHeaps[] = { m_boneMatrixArrayDH.Get() };
					commandList->SetDescriptorHeaps(1, descHeaps);
					descriptorHeapH = m_boneMatrixArrayDH.GetGPUHandle();
					commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);
				}

				// インスタンシングが有効なら、ワールド行列の配列をセット
				if (m_worldMatrixArrayDH.IsValid())
				{
					ID3D12DescriptorHeap* descHeaps[] = { m_worldMatrixArrayDH.Get() };
					commandList->SetDescriptorHeaps(1, descHeaps);
					descriptorHeapH = m_worldMatrixArrayDH.GetGPUHandle();
					commandList->SetGraphicsRootDescriptorTable(2, descriptorHeapH);
				}

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
					commandList->SetGraphicsRootDescriptorTable(3, descriptorHeapH);

					// 頂点バッファとインデックスバッファをセット
					commandList->SetIndexBuffer(*indexBuffer);
					commandList->SetVertexBuffer(*vertexBuffer);

					// 描画
					commandList->DrawInstanced(meshInfo.numIndices, m_fixNumInstanceOnFrame);

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
				m_isImportedModelScene = false;
				m_loadingState = EnLoadingState::enBeforeLoading;

				for (auto& geomData : m_geometryDataArray)
				{
					if (geomData)
					{
						delete geomData;
						geomData = nullptr;
					}
				}
				m_geometryDataArray.clear();

				m_boneMatrixArrayDH.Release();
				m_boneMatrixArraySB.Release();
				m_worldMatrixArrayDH.Release();
				m_worldMatrixArraySB.Release();
				for (auto& materialDH : m_materialDHs)
				{
					if (materialDH)
					{
						materialDH->Release();
						delete materialDH;
					}
				}
				
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
				m_modelDH.Release();
				m_modelCB.Release();
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
					delete m_animator;
					m_animator = nullptr;
				}
				return;
			}

			bool CBasicModelRenderer::Init(const nsRenderers::SModelInitData& modelInitData) noexcept
			{
				m_isImportedModelScene = false;
				m_modelInitDataRef = &modelInitData;

				if (modelInitData.GetFlags(EnModelInitDataFlags::enLoadingAsynchronous))
				{
					m_loadingState = EnLoadingState::enNowLoading;
					nsThread::CLoadModelThread::GetInstance()->PushLoadModelProcess(
						nsThread::CLoadModelThread::EnLoadProcessType::enLoadModel,
						this
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

				if (nsAssimpCommon::ImportScene(
					modelInitData.modelFilePath,
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

			bool CBasicModelRenderer::InitAsynchronous() noexcept
			{
				if (nsAssimpCommon::ImportScene(
					m_modelInitDataRef->modelFilePath,
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
				CreateModelCBV();
				CreateMaterialSRV();
				CreateBoneMatrisArraySB();
				CreateWorldMatrixArraySB(modelInitData);

				m_geometryDataArray.reserve(modelInitData.maxInstance);
				m_geometryDataArray.emplace_back(new nsGeometries::CGeometryData());
				m_geometryDataArray[0]->Init(dstMeshes);
				for (unsigned int instanceIdx = 1; instanceIdx < modelInitData.maxInstance; instanceIdx++)
				{
					m_geometryDataArray.emplace_back(new nsGeometries::CGeometryData());
					m_geometryDataArray[instanceIdx]->Init(m_geometryDataArray[0]->GetAABB());
				}

				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				if (modelInitData.rendererType != 
					nsRenderers::CRendererTable::EnRendererType::enBasicModel)
				{
					// 初期化データにレンダラータイプが設定されていたら、それを優先する。
					SetRenderType(modelInitData.rendererType);
				}
				else if (modelInitData.maxInstance > 2)
				{
					// インスタンシングが有効なら、インスタンシングタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enInstancingNonCullingModel);
					}
					else
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enInstancingModel);
					}
				}
				else if (IsSkeltalAnimationValid())
				{
					// スキンアニメーションが有効ならスキンタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enSkinNonCullingModel);
					}
					else
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enSkinModel);
					}
				}
				else
				{
					// その他は基本モデルタイプ
					if (modelInitData.GetFlags(EnModelInitDataFlags::enCullingOff))
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enBasicNonCullingModel);
					}
					else
					{
						SetRenderType(nsRenderers::CRendererTable::EnRendererType::enBasicModel);
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
					m_animator = new nsAnimations::CAnimator();
					isSkeltalAnimation = 
						m_animator->Init(
							modelInitData.animInitData,
							m_skelton, 
							modelInitData.GetFlags(EnModelInitDataFlags::enLoadingAsynchronous),
							modelInitData.GetFlags(EnModelInitDataFlags::enRegisterAnimationBank)
						);
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
						modelInitData.GetFlags(EnModelInitDataFlags::enNodeTransform)
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
					texture->Init(texFilePath.c_str());

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

				m_modelCB.Init(static_cast<unsigned int>(cbSize), L"ModelCB");

				const auto& mWorld = nsMath::CMatrix::Identity();
				const auto& mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				auto* mappedCB =
					static_cast<nsMath::CMatrix*>(m_modelCB.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				mappedCB[1] = mViewProj;

				// ディスクリプタヒープ作成
				constexpr unsigned int kNumDescHeaps = 1;
				m_modelDH.InitAsCbvSrvUav(kNumDescHeaps, L"ModelCB");

				// 定数バッファビュー作成
				m_modelCB.CreateConstantBufferView(m_modelDH.GetCPUHandle());

				return true;
			}


			bool CBasicModelRenderer::CreateMaterialSRV()
			{
				constexpr unsigned int numDescHeaps = 2;
				auto* device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				const auto inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav();

				unsigned int kNumTextures = static_cast<unsigned int>(m_diffuseTextures.size());
				m_materialDHs.reserve(kNumTextures);
				for (unsigned int texIdx = 0; texIdx < kNumTextures; texIdx++)
				{
					auto* materialDH = new nsDx12Wrappers::CDescriptorHeap();

					materialDH->InitAsCbvSrvUav(numDescHeaps, L"MaterialDH");
					auto matDescHandle = materialDH->GetCPUHandle();

					matSRVDesc.Format = m_diffuseTextures[texIdx]->GetResource()->GetDesc().Format;
					device->CreateShaderResourceView(
						m_diffuseTextures[texIdx]->GetResource(),
						&matSRVDesc,
						matDescHandle
					);

					matDescHandle.ptr += inc;

					matSRVDesc.Format = m_normalTextures[texIdx]->GetResource()->GetDesc().Format;
					device->CreateShaderResourceView(
						m_normalTextures[texIdx]->GetResource(),
						&matSRVDesc,
						matDescHandle
					);

					m_materialDHs.emplace_back(materialDH);

				}

				return true;
			}

			bool CBasicModelRenderer::CreateBoneMatrisArraySB()
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return true;
				}

				const auto& boneInfoArray = m_skelton->GetBoneInfoArray();
				unsigned int numBoneInfoArray = static_cast<unsigned int>(boneInfoArray.size());
				m_boneMatrices.resize(numBoneInfoArray);


				m_boneMatrixArrayDH.InitAsCbvSrvUav(1, L"BoneMatrixArrayDH");

				bool res =
					m_boneMatrixArraySB.Init(sizeof(nsMath::CMatrix), numBoneInfoArray);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_boneMatrixArraySBの生成に失敗しました。");
					return false;
				}

				m_boneMatrixArraySB.RegistShaderResourceView(m_boneMatrixArrayDH.GetCPUHandle());

				return true;
			}


			bool CBasicModelRenderer::CreateWorldMatrixArraySB(
				const nsRenderers::SModelInitData& modelInitData)
			{
				if (modelInitData.maxInstance <= 1)
				{
					return true;
				}


				m_worldMatrixArrayDH.InitAsCbvSrvUav(1, L"WorldMatrixArrayDH");

				bool res = 
					m_worldMatrixArraySB.Init(sizeof(nsMath::CMatrix), modelInitData.maxInstance);

				if (res != true)
				{
					nsGameWindow::MessageBoxError(L"m_worldMatrixArraySBの生成に失敗しました。");
					return false;
				}

				m_worldMatrixArraySB.RegistShaderResourceView(m_worldMatrixArrayDH.GetCPUHandle());

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
				mappedCB[1] = mViewProj;


				m_fixNumInstanceOnFrame = 0;

				// ジオメトリデータを更新
				m_geometryDataArray[0]->Update(m_worldMatrix);
				if (m_geometryDataArray[0]->IsInViewFrustum())
				{
					m_fixNumInstanceOnFrame++;
				}

				return;
			}

			void CBasicModelRenderer::UpdateAnimation(float deltaTime) noexcept
			{
				if (IsSkeltalAnimationValid() != true)
				{
					return;
				}

				m_animator->UpdateAnimation(deltaTime);

				if (m_geometryDataArray[0]->IsInViewFrustum())
				{
					m_animator->CalcAndGetAnimatedBoneTransforms(&m_boneMatrices);
					m_boneMatrixArraySB.CopyToMappedStructuredBuffer(m_boneMatrices.data());
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
				if (m_worldMatrixArrayDH.IsValid() != true)
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

				return;
			}


		}
	}
}