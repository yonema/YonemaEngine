#include "FBXRenderer.h"
#include "../GraphicsEngine.h"
#include "../Utils/AlignSize.h"
#include "../Utils/StringManipulation.h"
#include <fbxsdk.h>


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			void CFBXRenderer::Draw(nsDx12Wrappers::CCommandList* commandList)
			{
				// ○定数バッファのディスクリプタヒープをセット
				ID3D12DescriptorHeap* modelDescHeaps[] = { m_descriptorHeap.Get() };
				commandList->SetDescriptorHeaps(1, modelDescHeaps);
				auto descriptorHeapH = m_descriptorHeap.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);



				const auto kNumBuufers = static_cast<int>(m_indexBuffers.size());
				for (int i = 0; i < kNumBuufers; i++)
				{
					ID3D12DescriptorHeap* materialDescHeaps[] = { m_materialDHs.at(i)->Get() };
					commandList->SetDescriptorHeaps(1, materialDescHeaps);
					descriptorHeapH = m_materialDHs.at(i)->GetGPUHandle();
					commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapH);

					commandList->SetIndexBuffer(*m_indexBuffers.at(i));
					commandList->SetVertexBuffer(*m_vertexBuffers.at(i));

					commandList->DrawInstanced(m_vertexBuffers.at(i)->GetNumVertices());

				}
				return;
			}

			CFBXRenderer::CFBXRenderer(const SModelInitData& modelInitData)
			{
				Init(modelInitData);
				return;
			}
			CFBXRenderer::~CFBXRenderer()
			{
				Terminate();
				return;
			}

			void CFBXRenderer::UpdateWorldMatrix(
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
				mWorld = mScale * mRot * mTrans;

				// 定数バッファにコピー。
				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_constantBuffer.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				return;
			}

			void CFBXRenderer::Terminate()
			{
				Release();
				return;
			}

			void CFBXRenderer::Release()
			{
				for (auto& diffuseTexture : m_diffuseTextures)
				{
					if (diffuseTexture.second)
					{
						delete diffuseTexture.second;
					}
				}
				for (auto& materialCB : m_materialCBTable)
				{
					if (materialCB.second)
					{
						delete materialCB.second;
					}
				}
				for (auto& materialDH : m_materialDHTable)
				{
					if (materialDH.second)
					{
						delete materialDH.second;
					}
				}
				m_descriptorHeap.Release();
				m_constantBuffer.Release();
				for (auto& vertexBuffer : m_vertexBuffers)
				{
					if (vertexBuffer)
					{
						delete vertexBuffer;
					}
				}
				for (auto& indexBuffer : m_indexBuffers)
				{
					if (indexBuffer)
					{
						delete indexBuffer;
					}
				}
				return;
			}

			bool CFBXRenderer::Init(const SModelInitData& modelInitData)
			{			
				// FbxImportやFbxSceneを作成するために必要な管理クラス。
				const auto fbxManager = fbxsdk::FbxManager::Create();

				auto ioSetting = fbxsdk::FbxIOSettings::Create(fbxManager, IOSROOT);
				fbxManager->SetIOSettings(ioSetting);

				// FbxファイルをImportするためのパーサークラス。
				auto fbxImporter = fbxsdk::FbxImporter::Create(fbxManager, "FbxImporter");

				if (fbxImporter == nullptr)
				{
					return false;
				}

				// Importerによって、分解されたFbxのデータを保存するためのクラス
				auto fbxScene = fbxsdk::FbxScene::Create(fbxManager, "FbxScene");

				if (fbxScene == nullptr)
				{
					fbxImporter->Destroy();
					return false;
				}

				// Fbxファイルの初期化（Fbxファイルを開く）
				if (fbxImporter->Initialize(modelInitData.modelFilePath, -1, fbxManager->GetIOSettings()) != true)
				{
					fbxImporter->Destroy();
					fbxScene->Destroy();
					return false;
				}

				// Importerが持ってるデータを分解してSceneに渡す。
				// @attention コストの高い処理！
				if (fbxImporter->Import(fbxScene/*, true*/) != true)
				{
					fbxImporter->Destroy();
					fbxScene->Destroy();
					return false;
				}

				// インポートしたため、Importerはもういらない。
				fbxImporter->Destroy();



				const auto rootNode = fbxScene->GetRootNode();
				if (rootNode == nullptr)
				{
					fbxImporter->Destroy();
					fbxScene->Destroy();
					return false;
				}


				fbxsdk::FbxGeometryConverter fbxConverter(fbxManager);
				bool res = fbxConverter.SplitMeshesPerMaterial(fbxScene, true);

				// 三角形ポリゴンに変換する。
				// 最初から三角形ポリゴンならする必要なし。
				// ポリゴンを三角形にする。
				// コストの高い処理。
				res = fbxConverter.Triangulate(fbxScene, true);

				const int matObjectCount = fbxScene->GetSrcObjectCount<fbxsdk::FbxSurfaceMaterial>();
				std::unordered_map<std::string, SFbxMaterial> fbxMaterials;
				//m_diffuseTextures.resize(matObjectCount);
				for (int i = 0; i < matObjectCount; i++)
				{
					LoadMaterial(
						fbxScene->GetSrcObject<fbxsdk::FbxSurfaceMaterial>(i), 
						&fbxMaterials, 
						&m_diffuseTextures,
						modelInitData.modelFilePath);
				}

				CreateMaterialCBVTable(fbxMaterials);

				const int meshObjectCount = fbxScene->GetSrcObjectCount<fbxsdk::FbxMesh>();
				std::vector<std::vector<SFbxVertex>> verticesArray;
				std::vector<std::vector<unsigned short>> indicesArray;
				verticesArray.resize(meshObjectCount);
				indicesArray.resize(meshObjectCount);
				m_materialDHs.resize(meshObjectCount);
				for (int i = 0; i < meshObjectCount; i++)
				{
					CreateMesh(
						fbxScene->GetSrcObject<fbxsdk::FbxMesh>(i),
						&verticesArray.at(i),
						&indicesArray.at(i),
						&m_materialDHs.at(i),
						modelInitData
					);
				}


				CreateVertexAndIndexBuffer(verticesArray, indicesArray);



				// Sceneからデータを取得したため、Sceneはもういらない。
				fbxScene->Destroy();

				ioSetting->Destroy();
				fbxManager->Destroy();


				CreateConstantBufferView();

				CreateShaderResourceView();

				return true;
			}

			void CFBXRenderer::LoadMaterial(
				fbxsdk::FbxSurfaceMaterial* material,
				std::unordered_map<std::string, SFbxMaterial>* pFbxMaterials,
				//nsDx12Wrappers::CTexture** ppTexture,
				std::unordered_map < std::string, nsDx12Wrappers::CTexture*>* pTextures,
				const char* filePath)
			{
				auto matProp = material->FindProperty(fbxsdk::FbxSurfaceMaterial::sAmbient);
				
				enum class EnMaterialOrder
				{
					enAmbient,
					enDiffuse,
					enSpecular,
					enNumOrders
				};
				constexpr int numOrders = static_cast<int>(EnMaterialOrder::enNumOrders);
				fbxsdk::FbxDouble3 colors[numOrders];
				fbxsdk::FbxDouble factors[numOrders];

				if (material->GetClassId().Is(fbxsdk::FbxSurfaceLambert::ClassId))
				{
					const char* checkElementPropList[numOrders]
					{
						fbxsdk::FbxSurfaceMaterial::sAmbient,
						fbxsdk::FbxSurfaceMaterial::sDiffuse,
						fbxsdk::FbxSurfaceMaterial::sSpecular
					};
					const char* checkFactorPropList[numOrders]
					{
						fbxsdk::FbxSurfaceMaterial::sAmbientFactor,
						fbxsdk::FbxSurfaceMaterial::sDiffuseFactor,
						fbxsdk::FbxSurfaceMaterial::sSpecularFactor,
					};

					for (int i = 0; i < numOrders; i++)
					{
						matProp = material->FindProperty(checkElementPropList[i]);
						if (matProp.IsValid())
						{
							colors[i] = matProp.Get<fbxsdk::FbxDouble3>();
						}
						else
						{
							colors[i] = fbxsdk::FbxDouble3(1.0, 1.0, 1.0);
						}

						matProp = material->FindProperty(checkFactorPropList[i]);
						if (matProp.IsValid())
						{
							factors[i] = matProp.Get<fbxsdk::FbxDouble>();
						}
						else
						{
							factors[i] = 1.0;
						}

					}

				}

				constexpr int orderAmbient = static_cast<int>(EnMaterialOrder::enAmbient);
				const auto* color = &colors[orderAmbient];
				const auto* factor = &factors[orderAmbient];
				SFbxMaterial fbxMaterial;
				fbxMaterial.ambient = 
				{ 
					static_cast<float>((*color)[0]),
					static_cast<float>((*color)[1]),
					static_cast<float>((*color)[2]),
					static_cast<float>(*factor)
				};

				constexpr int orderDiffuse = static_cast<int>(EnMaterialOrder::enDiffuse);
				color = &colors[orderDiffuse];
				factor = &factors[orderDiffuse];
				fbxMaterial.diffuse =
				{
					static_cast<float>((*color)[0]),
					static_cast<float>((*color)[1]),
					static_cast<float>((*color)[2]),
					static_cast<float>(*factor)
				};

				constexpr int orderSpecular = static_cast<int>(EnMaterialOrder::enSpecular);
				color = &colors[orderSpecular];
				factor = &factors[orderSpecular];
				fbxMaterial.specular =
				{
					static_cast<float>((*color)[0]),
					static_cast<float>((*color)[1]),
					static_cast<float>((*color)[2]),
					static_cast<float>(*factor)
				};

				pFbxMaterials->emplace(material->GetName(), fbxMaterial);




				matProp = material->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
				fbxsdk::FbxFileTexture* texture = nullptr;
				if (matProp.IsValid() != true)
				{
					return;
				}


				int textureCount = matProp.GetSrcObjectCount<fbxsdk::FbxFileTexture>();
				int layerCount = matProp.GetSrcObjectCount<fbxsdk::FbxLayeredTexture>();

				if (textureCount > 0)
				{
					texture = matProp.GetSrcObject<fbxsdk::FbxFileTexture>();
				}
				else
				{
					//int layerCount = matProp.GetSrcObjectCount<fbxsdk::FbxLayeredTexture>();
					if (layerCount > 0)
					{
						texture = matProp.GetSrcObject<fbxsdk::FbxFileTexture>();
					}

				}

				pTextures->emplace(material->GetName(), new nsDx12Wrappers::CTexture());
				auto& pTex = pTextures->at(material->GetName());
				if (texture != nullptr)
				{
					const char* texRelativeFileName = texture->GetRelativeFileName();
					auto texFileName = nsUtils::GetFileNameFromFilePath(texRelativeFileName);

					// 文字化け対策
					char* fbxFileName;
					size_t size = 0;
					fbxsdk::FbxUTF8ToAnsi(texFileName.c_str(), fbxFileName, &size);
					texFileName = fbxFileName;
					fbxsdk::FbxFree(fbxFileName);

					texFileName = "Textures/" + texFileName;
					auto texFilePath = 
						nsUtils::GetTexturePathFromModelAndTexPath(filePath, texFileName.c_str());

					pTex->Init(texFilePath.c_str());
				}
				else
				{
					pTex = nullptr;
				}

				return;
			}


			void CFBXRenderer::CreateMesh(
				const fbxsdk::FbxMesh* mesh,
				std::vector<SFbxVertex>* pVertices,
				std::vector<unsigned short>* pIndices,
				nsDx12Wrappers::CDescriptorHeap** ppMaterialDH,
				const SModelInitData& modelInitData
			)
			{
				// fbxの頂点バッファ
				const auto kFbxVertices = mesh->GetControlPoints();
				// fbxのインデックスバッファ
				const auto kFbxIndices = mesh->GetPolygonVertices();
				// fbxの頂点数
				const auto kFbxPolygonVertexCount = mesh->GetPolygonVertexCount();
				// fbxのワールド行列。
				const auto& kFbxMWorld = mesh->GetNode()->EvaluateGlobalTransform();

				// ワールド行列を使いやすいように、このエンジンの行列クラスにコピーする。
				nsMath::CMatrix fbxMWolrd;
				for (int y = 0; y < 4; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						fbxMWolrd.m_fMat[y][x] = static_cast<float>(kFbxMWorld[y][x]);
					}
				}



				// 単位をそろえるため、平行移動量を100.0fで割る。
				fbxMWolrd.m_fMat[3][0] /= 100.0f;
				fbxMWolrd.m_fMat[3][1] /= 100.0f;
				fbxMWolrd.m_fMat[3][2] /= 100.0f;
				
				nsMath::CMatrix mTrans;
				mTrans.m_fMat[3][0] = fbxMWolrd.m_fMat[3][0];
				mTrans.m_fMat[3][1] = fbxMWolrd.m_fMat[3][1];
				mTrans.m_fMat[3][2] = -fbxMWolrd.m_fMat[3][2];

				fbxMWolrd.m_vec4Mat[3] = nsMath::CVector4::Identity();

				nsMath::CVector3 scale;
				nsMath::CMatrix mScale;
				// 行列から拡大率を取得する。
				scale.x = fbxMWolrd.m_vec4Mat[0].Length();
				scale.y = fbxMWolrd.m_vec4Mat[2].Length();
				scale.z = fbxMWolrd.m_vec4Mat[1].Length();
				scale.Scale(0.01f);
				mScale.MakeScaling(scale);

				// 行列から拡大率と平行移動量を除去して回転量を取得する。
				fbxMWolrd.m_vec4Mat[0].Normalize();
				fbxMWolrd.m_vec4Mat[1].Normalize();
				fbxMWolrd.m_vec4Mat[2].Normalize();
				nsMath::CMatrix mRot = fbxMWolrd;
				if (modelInitData.isVertesTranspos)
				{
					mRot.Transpose();
				}
				nsMath::CMatrix mBias;
				mBias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				nsMath::CMatrix mWorld = mBias * mScale * mRot * mTrans;


				// 〇頂点バッファのデータをコピーする。
				std::vector<SFbxVertex> fbxVertex;
				pVertices->resize(kFbxPolygonVertexCount);

				//・座標のコピー。
				for (int i = 0; i < kFbxPolygonVertexCount; i++)
				{
					unsigned short index = static_cast<unsigned short>(kFbxIndices[i]);

					// 座標をコピー
					// ・右手系(Fbx)から左手系(DirectX)に変換するため、X軸を反転。
					// ・Zup(Fbx)からYup(DirectX)に変換するため、
					// 　Y = fbxZ
					// 　Z = -fbxY
					// 　を入れる。 

					pVertices->at(i).position.x =
						static_cast<float>(-kFbxVertices[index][0]);
					pVertices->at(i).position.y =
						static_cast<float>(kFbxVertices[index][2]);
					pVertices->at(i).position.z =
						static_cast<float>(-kFbxVertices[index][1]);

					mWorld.Apply(pVertices->at(i).position);
				}

				mWorld.Inverse();
				mWorld.Transpose();

				// ・法線のコピー。
				fbxsdk::FbxArray<fbxsdk::FbxVector4> fbxNormals;
				mesh->GetPolygonVertexNormals(fbxNormals);

				// 法線は頂点とは違い、インデックスバッファの順番で作られているので
				// そのままの順番でコピーする。

				for (int i = 0; i < fbxNormals.Size(); i++)
				{
					// 法線をコピー
					// 右手系(Fbx)から左手系(DirectX)に変換するため、X軸を反転。
					// ・Zup(Fbx)からYup(DirectX)に変換するため、
					// 　Y = fbxZ
					// 　Z = -fbxY
					// 　を入れる。 
					pVertices->at(i).normal.x =
						-static_cast<float>(fbxNormals[i][0]);
					pVertices->at(i).normal.y =
						static_cast<float>(fbxNormals[i][2]);
					pVertices->at(i).normal.z =
						static_cast<float>(-fbxNormals[i][1]);

					mWorld.Apply(pVertices->at(i).normal);
				}

				// ・頂点カラーのコピー。
				const fbxsdk::FbxGeometryElementVertexColor* vertexColor = nullptr;
				if (mesh->GetElementVertexColorCount() > 0)
				{
					vertexColor = mesh->GetElementVertexColor();
				}

				if (vertexColor != nullptr)
				{
					if (vertexColor->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex &&
						vertexColor->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect)
					{
						const auto& vcArray = vertexColor->GetDirectArray();
						const auto& vcIndices = vertexColor->GetIndexArray();

						const int vcIndicesCount = vcIndices.GetCount();
						for (int i = 0; i < vcIndicesCount; i++)
						{
							int id = vcIndices.GetAt(i);
							const auto& color = vcArray.GetAt(id);

							pVertices->at(i).color.r = static_cast<float>(color.mRed);
							pVertices->at(i).color.g = static_cast<float>(color.mGreen);
							pVertices->at(i).color.b = static_cast<float>(color.mBlue);
							pVertices->at(i).color.a = static_cast<float>(color.mAlpha);
						}
					}

				}

				// ・UVのコピー。
				fbxsdk::FbxStringList uvSetNames;
				mesh->GetUVSetNames(uvSetNames);

				fbxsdk::FbxArray<FbxVector2> uvBuffer;
				// UVSetの名前からUVSetを取得する
				// 今回はマルチテクスチャには対応しないので最初の名前を使う
				mesh->GetPolygonVertexUVs(uvSetNames.GetStringAt(0), uvBuffer);

				for (int i = 0; i < uvBuffer.Size(); i++)
				{
					const auto& uv = uvBuffer[i];

					pVertices->at(i).uv.x = static_cast<float>(uv[0]);
					// Vは反転させる
					pVertices->at(i).uv.y = (1.0f - static_cast<float>(uv[1]));
					//pVertices->at(i).uv.y = static_cast<float>(uv[1]);
				}


				// 〇インデックスバッファを作り直す。

				// fbxのポリゴン数
				const auto kFbxPolygonCount = mesh->GetPolygonCount();
				// インデックスバッファの数は、ポリゴン数 * 1ポリゴンの頂点数(3)
				pIndices->resize(kFbxPolygonCount * 3);

				for (int i = 0; i < kFbxPolygonCount; i++)
				{
					// インデックスバッファの作成。
					// 右手系(Fbx)から左手系(DirectX)に変換するため、逆回りにする。
					pIndices->at(i * 3) = i * 3 + 2;
					pIndices->at(i * 3 + 1) = i * 3 + 1;
					pIndices->at(i * 3 + 2) = i * 3;
				}


				if (mesh->GetElementMaterialCount() == 0)
				{
					*ppMaterialDH = m_materialDHTable.at("");
					return;
				}

				const fbxsdk::FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
				int index = material->GetIndexArray().GetAt(0);
				auto surfaceMaterial = mesh->GetNode()->GetSrcObject<fbxsdk::FbxSurfaceMaterial> (index);
				if (surfaceMaterial != nullptr)
				{
					const auto& name = surfaceMaterial->GetName();
					*ppMaterialDH = m_materialDHTable.at(surfaceMaterial->GetName());
				}
				else
				{
					*ppMaterialDH = m_materialDHTable.at("");
				}

				return;
			}

			bool CFBXRenderer::CreateVertexAndIndexBuffer(
				const std::vector<std::vector<SFbxVertex>>& verticesArray,
				const std::vector<std::vector<unsigned short>>& indicesArray
			)
			{
				const auto alignedStrideSize = nsUtils::AlignSize(sizeof(SFbxVertex), 4);
				const int kNumBuffers = static_cast<int>(verticesArray.size());
				m_vertexBuffers.resize(kNumBuffers);
				m_indexBuffers.resize(kNumBuffers);

				for (int i = 0; i < kNumBuffers; i++)
				{
					m_vertexBuffers.at(i) = new nsDx12Wrappers::CVertexBuffer();
					auto& vertices = verticesArray.at(i);
					auto resV = m_vertexBuffers.at(i)->Init(
						static_cast<unsigned int>(alignedStrideSize * vertices.size()),
						alignedStrideSize,
						&vertices.at(0)
					);

					m_indexBuffers.at(i) = new nsDx12Wrappers::CIndexBuffer();
					auto indices = indicesArray.at(i);
					auto resI = m_indexBuffers.at(i)->Init(
						static_cast<unsigned int>(sizeof(indices.at(0)) * indices.size()),
						&indices.at(0)
					);
					if (resV && resI != true)
					{
						return false;
					}
				}

				return true;
			}

			bool CFBXRenderer::CreateConstantBufferView()
			{
				// 〇定数バッファ作成
				auto cbSize = sizeof(nsMath::CMatrix) * 2;
				m_constantBuffer.Init(static_cast<unsigned int>(cbSize), L"FBXModel");

				nsMath::CMatrix mTrans;
				mTrans.MakeTranslation(0.0f, 2.0f, -1.0f);
				nsMath::CQuaternion qRot;
				qRot.SetRotationYDeg(-180.0f);
				nsMath::CMatrix mRot;
				mRot.MakeRotationFromQuaternion(qRot);
				nsMath::CMatrix mScale;
				constexpr float scaling = 10.0f;
				mScale.MakeScaling(scaling, scaling, scaling);
				nsMath::CMatrix mWorld = mScale * mRot * mTrans;

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_constantBuffer.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				// 〇ディスクリプタヒープ作成
				constexpr unsigned int numDescHeaps = 1;
				m_descriptorHeap.InitAsCbvSrvUav(numDescHeaps, L"FBXModel");

				// 〇定数バッファビュー作成
				m_constantBuffer.CreateConstantBufferView(m_descriptorHeap.GetCPUHandle());




				return true;
			}

			bool CFBXRenderer::CreateMaterialCBVTable(
				const std::unordered_map<std::string, SFbxMaterial>& fbxMaterials)
			{
				auto cbSize = static_cast<unsigned int>(sizeof(SFbxMaterial));
				// 定数バッファ1 + シェーダーリソース1 = 2
				constexpr unsigned int numDescHeaps = 1 + 1;
				const auto inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav();
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				//auto diffuseTexItr = m_diffuseTextures.begin();
				auto whiteTex = CGraphicsEngine::GetInstance()->GetWhiteTexture();


				D3D12_SHADER_RESOURCE_VIEW_DESC matSRVDesc = {};
				matSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				matSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				matSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				matSRVDesc.Texture2D.MipLevels = 1;

				for (const auto& fbxMaterial : fbxMaterials)
				{
					// 〇ディスクリプタヒープ作成
					m_materialDHTable.emplace(fbxMaterial.first, new nsDx12Wrappers::CDescriptorHeap());
					m_materialDHTable.at(fbxMaterial.first)->InitAsCbvSrvUav(
						numDescHeaps, L"FbxMaterialDH");
					auto matDescHandle = m_materialDHTable.at(fbxMaterial.first)->GetCPUHandle();

					// 〇定数バッファ作成
					m_materialCBTable.emplace(fbxMaterial.first, new nsDx12Wrappers::CConstantBuffer());
					m_materialCBTable.at(fbxMaterial.first)->Init(
						cbSize, L"FbxMaterialCB", 1, &fbxMaterial.second);
					m_materialCBTable.at(fbxMaterial.first)->Unmap();

					// 〇定数バッファビュー作成
					m_materialCBTable.at(fbxMaterial.first)->CreateConstantBufferView(
						matDescHandle);

					matDescHandle.ptr += inc;

					
					auto& diffuseTexture = m_diffuseTextures.at(fbxMaterial.first);
					// 〇シェーダーリソースビュー作成
					if (diffuseTexture)
					{
						matSRVDesc.Format = diffuseTexture->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							diffuseTexture->GetResource(),
							&matSRVDesc,
							matDescHandle
						);

					}
					else
					{
						matSRVDesc.Format = whiteTex->GetResource()->GetDesc().Format;
						device->CreateShaderResourceView(
							whiteTex->GetResource(),
							&matSRVDesc,
							matDescHandle
						);
					}

					matDescHandle.ptr += inc;
				}

				// ヌルマテリアル作成
				SFbxMaterial nullFbxMaterial;
				nullFbxMaterial.diffuse = { 0.909f, 0.56f,0.8f, 1.0f };

				// 〇ディスクリプタヒープ作成
				m_materialDHTable.emplace("", new nsDx12Wrappers::CDescriptorHeap());
				m_materialDHTable.at("")->InitAsCbvSrvUav(
					numDescHeaps, L"FbxNullMaterialDH");

				auto matDescHandle = m_materialDHTable.at("")->GetCPUHandle();

				// 〇定数バッファ作成
				m_materialCBTable.emplace("", new nsDx12Wrappers::CConstantBuffer());
				m_materialCBTable.at("")->Init(
					cbSize, L"FbxNullMaterialCB", 1, &nullFbxMaterial);
				m_materialCBTable.at("")->Unmap();

				// 〇定数バッファビュー作成
				m_materialCBTable.at("")->CreateConstantBufferView(
					matDescHandle);

				matDescHandle.ptr += inc;

				// 〇シェーダーリソースビュー作成
				matSRVDesc.Format = whiteTex->GetResource()->GetDesc().Format;
				device->CreateShaderResourceView(
					whiteTex->GetResource(),
					&matSRVDesc,
					matDescHandle
				);
;
				return true;
			}

			bool CFBXRenderer::CreateShaderResourceView()
			{


				return true;
			}


			
		}
	}
}