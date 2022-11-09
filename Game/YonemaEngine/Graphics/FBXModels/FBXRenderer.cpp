#include "FBXRenderer.h"
#include "../GraphicsEngine.h"
#include "../Utils/AlignSize.h"
#include "../Utils/StringManipulation.h"


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


				// マテリアルごとに描画
				// 最初にマテリアルごとにメッシュ分解しているため、メッシュごとに描画と同意。
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

			void CFBXRenderer::UpdateAnimation(float deltaTime)
			{
				return;
				int frameNo = 0;
				for (const auto& animationData : m_animationData)
				{
					m_boneMatrix.at(m_boneNameTable[animationData.first]) = 
						animationData.second.at(frameNo);
				}
				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_constantBuffer.GetMappedConstantBuffer());

				copy(m_boneMatrix.begin(), m_boneMatrix.end(), mappedCB + 2);
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
				fbxsdk::FbxManager* fbxManager = nullptr;
				fbxsdk::FbxScene* fbxScene = nullptr;

				if (InitializeSdkObjects(fbxManager, fbxScene) != true)
				{
					return false;
				}

				fbxsdk::FbxImporter* fbxImporter = nullptr;
				fbxImporter = fbxsdk::FbxImporter::Create(fbxManager, "MyFbxImporter");
				if (fbxImporter == nullptr)
				{
					DestroySdkObjects(fbxManager, false);
					return false;
				}

				int fileFormat = -1;
				// ファイルに指定されたインポート（リーダー）ファイル形式を検出する
				if (fbxManager->GetIOPluginRegistry()->
					DetectReaderFileFormat(modelInitData.modelFilePath, fileFormat) != true)
				{
					// 検出できないファイル形式です。 FbxImporter::eFBX_BINARY 形式でトライします。
					fileFormat = fbxManager->GetIOPluginRegistry()->
						FindReaderIDByDescription("FBX binary (*.fbx)");
				}

				// Fbxファイルの初期化（Fbxファイルを開く）
				if (fbxImporter->Initialize(
					modelInitData.modelFilePath, fileFormat, fbxManager->GetIOSettings()) != true)
				{
					DestroySdkObjects(fbxManager, false);
					return false;
				}


				// Importerが持ってるデータを分解してSceneに渡す。
				// @attention コストの高い処理！
				if (fbxImporter->Import(fbxScene/*, true*/) != true)
				{
					std::string str = "Unable to import file ";
					str += modelInitData.modelFilePath;
					str += "\nError reported: ";
					str += fbxImporter->GetStatus().GetErrorString();

					::OutputDebugStringA("\n");
					::OutputDebugStringA("********************************************************************************\n");
					::OutputDebugStringA(str.c_str());
					::OutputDebugStringA("\n");
					::OutputDebugStringA("********************************************************************************\n");
					::OutputDebugStringA("\n");

					DestroySdkObjects(fbxManager, false);
					return false;
				}

				// 整合性をチェック
				fbxsdk::FbxStatus status;
				fbxsdk::FbxArray<fbxsdk::FbxString*> details;
				fbxsdk::FbxSceneCheckUtility sceneCheck(
					fbxsdk::FbxCast<fbxsdk::FbxScene>(fbxScene), &status, &details);
				bool lNotify = (!sceneCheck.Validate(fbxsdk::FbxSceneCheckUtility::eCkeckData) &&
					details.GetCount() > 0) || 
					(fbxImporter->GetStatus().GetCode() != fbxsdk::FbxStatus::eSuccess);
				if (lNotify)
				{
					::OutputDebugStringA("\n");
					::OutputDebugStringA("********************************************************************************\n");
					if (details.GetCount())
					{
						::OutputDebugStringA("Scene integrity verification failed with the following errors:\n");

						for (int i = 0; i < details.GetCount(); i++)
						{
							std::string str = "   ";
							str += details[i]->Buffer();
							str += "\n";
							::OutputDebugStringA(str.c_str());
						}

						fbxsdk::FbxArrayDelete<fbxsdk::FbxString*>(details);
					}

					if (fbxImporter->GetStatus().GetCode() != fbxsdk::FbxStatus::eSuccess)
					{
						::OutputDebugStringA("\n");
						::OutputDebugStringA("WARNING:\n");
						::OutputDebugStringA("   The importer was able to read the file but with errors.\n");
						::OutputDebugStringA("   Loaded scene may be incomplete.\n\n");
						std::string str = "   Last error message:";
						str += fbxImporter->GetStatus().GetErrorString();
						str += "\n";
						::OutputDebugStringA(str.c_str());
					}

					::OutputDebugStringA("********************************************************************************\n");
					::OutputDebugStringA("\n");
				}

				// 必要に応じて軸変換を行う。

				// Fbxファイルに設定してある軸設定
				fbxsdk::FbxAxisSystem SceneAxisSystem = 
					fbxScene->GetGlobalSettings().GetAxisSystem();
				// このゲームの軸設定
				fbxsdk::FbxAxisSystem OurAxisSystem(fbxsdk::FbxAxisSystem::DirectX);
				//fbxsdk::FbxAxisSystem OurAxisSystem(
				//	fbxsdk::FbxAxisSystem::eYAxis,
				//	fbxsdk::FbxAxisSystem::eParityOdd,
				//	fbxsdk::FbxAxisSystem::eRightHanded);

				if (SceneAxisSystem != OurAxisSystem)
				{
					// 軸設定に差異があれば軸変換を行う。
					OurAxisSystem.ConvertScene(fbxScene);
				}

				// 必要に応じて単位変換を行う。

				// Fbxファイルに設定してある単位設定
				fbxsdk::FbxSystemUnit SceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
				// センチメートルに対する相対的な倍率を調べる。
				if (SceneSystemUnit.GetScaleFactor() != 1.0)
				{
					// 目的の単位設定でなかったなら、単位変換を行う。
					fbxsdk::FbxSystemUnit::cm.ConvertScene(fbxScene);
				}

				// メッシュ、NURBS、パッチを、三角メッシュに変換する。
				// コストの高い処理。
				fbxsdk::FbxGeometryConverter geometryConverter(fbxManager);
				bool res = false;
				try
				{
					res = geometryConverter.Triangulate(fbxScene, true);
				}
				catch (std::runtime_error) 
				{
					::OutputDebugStringA("Scene integrity verification failed.\n");
					return false;
				}

				unsigned int totalFrames = 0;
				
				auto timeMode = fbxsdk::FbxGetGlobalTimeMode();
				fbxsdk::FbxTime oneFrameTime, startTime, stopTime;
				oneFrameTime.SetTime(0, 0, 0, 1, 0, timeMode);
				fbxsdk::FbxArray<fbxsdk::FbxString*> animStackNameArray;
				fbxScene->FillAnimStackNameArray(animStackNameArray);
				const auto animStackCount = animStackNameArray.GetCount();

				for (int i = 0; i < animStackCount; i++)
				{
					auto takeInfo = fbxScene->GetTakeInfo(*(animStackNameArray[i]));
					if (takeInfo)
					{
						startTime = takeInfo->mLocalTimeSpan.GetStart().Get();
						stopTime = takeInfo->mLocalTimeSpan.GetStop().Get();
						auto totalTime = (stopTime - startTime) / oneFrameTime;
						totalFrames = static_cast<unsigned int>(totalTime.Get());
						break;
					}
				}

				// インポートしたため、Importerはもういらない。
				fbxImporter->Destroy();
				fbxImporter = nullptr;



				const auto rootNode = fbxScene->GetRootNode();
				if (rootNode == nullptr)
				{
					DestroySdkObjects(fbxManager, false);
					return false;
				}


				res = geometryConverter.SplitMeshesPerMaterial(fbxScene, true);


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
				m_weightTableArray.resize(meshObjectCount);
				//m_boneMatrixArray.resize(meshObjectCount);
				m_materialDHs.resize(meshObjectCount);
				for (int i = 0; i < meshObjectCount; i++)
				{
					CreateMesh(
						static_cast<unsigned int>(i),
						fbxScene->GetSrcObject<fbxsdk::FbxMesh>(i),
						&verticesArray,
						&indicesArray,
						startTime,
						oneFrameTime,
						totalFrames,
						modelInitData
					);
				}


				CreateVertexAndIndexBuffer(verticesArray, indicesArray);



				// Sceneからデータを取得したため、Sceneはもういらない。
				DestroySdkObjects(fbxManager, true);

				CreateConstantBufferView();

				CreateShaderResourceView();

				return true;
			}


			bool CFBXRenderer::InitializeSdkObjects(
				fbxsdk::FbxManager*& pManager, fbxsdk::FbxScene*& pScene)
			{
				pManager = fbxsdk::FbxManager::Create();
				if (pManager == nullptr)
				{
					::OutputDebugStringA("Error: Unable to create FBX Manager!\n");
					nsGameWindow::MessageBoxWarning(L"FbxManagerが生成できませんでした。");
					return false;
				}

				// fbxのバージョンを表示
				std::string str("Autodesk FBX SDK version ");
				str += pManager->GetVersion();
				str += "\n";
				::OutputDebugStringA(str.c_str());

				fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(pManager, IOSROOT);
				pManager->SetIOSettings(ios);

				// 実行ディレクトリからプラグインを読み込む（任意）
				fbxsdk::FbxString lPath = fbxsdk::FbxGetApplicationDirectory();
				pManager->LoadPluginsDirectory(lPath.Buffer());

				pScene = fbxsdk::FbxScene::Create(pManager, "MyFbxScene");
				if (pScene == nullptr)
				{
					::OutputDebugStringA("Error: Unable to create FBX scene!\n");
					nsGameWindow::MessageBoxWarning(L"FbxSceneが生成できませんでした。");
					return false;
				}

				return true;
			}

			void CFBXRenderer::DestroySdkObjects(fbxsdk::FbxManager* pManager, bool exitStatus)
			{
				if (pManager)
				{
					// FbxManagerを削除します。
					// FbxManagerを使用して割り当てられたオブジェクトで、
					// 明示的に破棄されていないものもすべて自動的に破棄されます。
					pManager->Destroy();
				}
				if (exitStatus)
				{
					::OutputDebugStringA("Program Success!\n");
				}
				return;
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
				unsigned int objectIdx,
				const fbxsdk::FbxMesh* mesh,
				std::vector<std::vector<SFbxVertex>>* pVerticesArray,
				std::vector<std::vector<unsigned short>>* pIndicesArray,
				const fbxsdk::FbxTime& startTime,
				const fbxsdk::FbxTime& oneFrameTime,
				const unsigned int totalFrames,
				const SModelInitData& modelInitData
			)
			{
				auto* pVertices = &pVerticesArray->at(objectIdx);
				auto* pIndices = &pIndicesArray->at(objectIdx);

				// fbxの頂点バッファ
				const auto kFbxVertices = mesh->GetControlPoints();
				// fbxのインデックスバッファ
				const auto kFbxIndices = mesh->GetPolygonVertices();
				// fbxの頂点数
				const auto kFbxPolygonVertexCount = mesh->GetPolygonVertexCount();
				// fbxのワールド行列。
				const auto& kFbxMWorld = mesh->GetNode()->EvaluateGlobalTransform();

				m_weightTableArray.at(objectIdx).resize(kFbxPolygonVertexCount);

				// ワールド行列を使いやすいように、このエンジンの行列クラスにコピーする。
				nsMath::CMatrix fbxMWolrd;
				for (int y = 0; y < 4; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						fbxMWolrd.m_fMat[y][x] = static_cast<float>(kFbxMWorld[y][x]);
					}
				}



				//// 単位をそろえるため、平行移動量を100.0fで割る。
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
					m_materialDHs.at(objectIdx) = m_materialDHTable.at("");
					return;
				}

				const fbxsdk::FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
				int index = material->GetIndexArray().GetAt(0);
				auto surfaceMaterial = mesh->GetNode()->GetSrcObject<fbxsdk::FbxSurfaceMaterial> (index);
				if (surfaceMaterial != nullptr)
				{
					const auto& name = surfaceMaterial->GetName();
					m_materialDHs.at(objectIdx) = m_materialDHTable.at(surfaceMaterial->GetName());
				}
				else
				{
					m_materialDHs.at(objectIdx) = m_materialDHTable.at("");
				}


				const int skinCount = mesh->GetDeformerCount(fbxsdk::FbxDeformer::EDeformerType::eSkin);
				auto timeMode = fbxsdk::FbxGetGlobalTimeMode();
				// 複数のスケルトンは未対応
				for (int skinIdx = 0; skinIdx < skinCount; skinIdx++)
				{
					fbxsdk::FbxSkin* skin = static_cast<fbxsdk::FbxSkin*>(
						mesh->GetDeformer(skinIdx, fbxsdk::FbxDeformer::EDeformerType::eSkin));

					const int clusterCount = skin->GetClusterCount();
					bool updateBoneMatrix = false;
					if (m_boneMatrix.empty())
					{
						m_boneMatrix.resize(clusterCount);
						updateBoneMatrix = true;
					}
					for (int clusterIdx = 0; clusterIdx < clusterCount; clusterIdx++)
					{
						// クラスタ(ボーン)
						auto cluster = skin->GetCluster(clusterIdx);

						const int cpIndicesCount = cluster->GetControlPointIndicesCount();
						const int* cpIndices = cluster->GetControlPointIndices();
						const double* cpWeights = cluster->GetControlPointWeights();
						int cpNum = 0;
						float totalWeight = 0;
						auto clusterName = cluster->GetName();
						if (cpIndicesCount < 0)
						{
							int a = 1;
						}
						for (int cpIndicesIdx = 0; cpIndicesIdx < cpIndicesCount; cpIndicesIdx++)
						{
							const int cpIndex = cpIndices[cpIndicesIdx];
							const float weight = static_cast<float>(cpWeights[cpIndicesIdx]);
							//if (weight > 0.0f)
							{
								if (m_weightTableArray.at(objectIdx).at(cpIndex).count(clusterName) > 0)
								{
									int a = 1;
								}
								m_weightTableArray.at(objectIdx).at(cpIndex).emplace(clusterName, weight);
							}
						}

						if (updateBoneMatrix)
						{
							if (m_boneNameTable.count(clusterName) == 0)
							{
								m_boneNameTable.emplace(clusterName, clusterIdx);
							}
							else
							{
								int a = 1;
							}

							fbxsdk::FbxAMatrix mTransfrom;
							cluster->GetTransformLinkMatrix(mTransfrom);

							for (int y = 0; y < 4; y++)
							{
								for (int x = 0; x < 4; x++)
								{
									// 初期ポーズを取得
									m_boneMatrix.at(clusterIdx).m_fMat[y][x] =
										static_cast<float>(mTransfrom[y][x]);
								}
							}
							m_boneMatrix.at(clusterIdx) = nsMath::CMatrix::Identity();
						}

						if (m_animationData.count(clusterName) == 0)
						{
							std::vector<nsMath::CMatrix> tempMat;
							m_animationData.emplace(clusterName, tempMat);
							auto& animData = m_animationData.at(clusterName);
							animData.resize(totalFrames);
							for (unsigned int frameIdx = 0; frameIdx < totalFrames; frameIdx++)
							{
								const auto& fbxMat = cluster->GetLink()->
									EvaluateGlobalTransform(oneFrameTime * frameIdx);
								auto& anim = animData.at(frameIdx);
								fbxMat[0][0];
								for (int y = 0; y < 4; y++)
								{
									for (int x = 0; x < 4; x++)
									{
										anim.m_fMat[y][x] = static_cast<float>(fbxMat[y][x]);
									}
								}
							}
						}
					}
				}

				for (int i = 0; i < kFbxPolygonVertexCount; i++)
				{
					int idx = 0;
					if (m_weightTableArray.at(objectIdx).at(i).empty())
					{
						int a = 1;
					}
					for (const auto& weightTable : m_weightTableArray.at(objectIdx).at(i))
					{
						if (idx > 3)
						{
							int a = 1;
							break;
						}

						pVertices->at(i).boneNo[idx] = m_boneNameTable[weightTable.first];
						pVertices->at(i).weights[idx] =
							static_cast<unsigned short>(weightTable.second * 10000.0f);
						idx++;

					}
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

				unsigned int boneNum = static_cast<unsigned int>(m_boneMatrix.size());
				cbSize += sizeof(nsMath::CMatrix) * boneNum;

				m_constantBuffer.Init(static_cast<unsigned int>(cbSize), L"FBXModel");

				nsMath::CMatrix mWorld = nsMath::CMatrix::Identity();

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_constantBuffer.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				copy(m_boneMatrix.begin(), m_boneMatrix.end(), mappedCB + 2);

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

				return true;
			}

			bool CFBXRenderer::CreateShaderResourceView()
			{


				return true;
			}


			
		}
	}
}