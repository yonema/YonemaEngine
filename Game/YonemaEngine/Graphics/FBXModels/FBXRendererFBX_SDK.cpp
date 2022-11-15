#include "FBXRendererFBX_SDK.h"
#include "../GraphicsEngine.h"
#include "../Utils/AlignSize.h"
#include "../Utils/StringManipulation.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			void CFBXAnimTimeInfo::Init(fbxsdk::FbxScene* fbxScene)
			{
				fbxsdk::FbxArray<fbxsdk::FbxString*> animStackNameArray;
				fbxScene->FillAnimStackNameArray(animStackNameArray);

				const auto animStackCount = animStackNameArray.GetCount();

				bool hasAnimStack = false;

				for (int i = 0; i < animStackCount; i++)
				{
					auto takeInfo = fbxScene->GetTakeInfo(animStackNameArray[i]->Buffer());
					if (takeInfo)
					{
						m_startTime = takeInfo->mLocalTimeSpan.GetStart().Get();
						m_stopTime = takeInfo->mLocalTimeSpan.GetStop().Get();
						hasAnimStack = true;
						break;
					}
				}

				if (hasAnimStack != true)
				{
					return;
				}


				auto timeMode = fbxsdk::FbxGetGlobalTimeMode();
				m_oneFrameTime.SetTime(0, 0, 0, 1, 0, timeMode);

				m_totalTime = (m_stopTime - m_startTime) / m_oneFrameTime;

				return;
			}

			fbxsdk::FbxTime CFBXAnimTimeInfo::GetTime(int frame) const noexcept
			{
				return m_startTime + m_oneFrameTime * frame;
			}


			fbxsdk::FbxAMatrix GetPoseMatrix(fbxsdk::FbxPose* pPose, int pNodeIndex)
			{
				fbxsdk::FbxAMatrix lPoseMatrix;
				fbxsdk::FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

				memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

				return lPoseMatrix;
			}

			fbxsdk::FbxAMatrix GetGlobalPosition(fbxsdk::FbxNode* pNode, const fbxsdk::FbxTime& pTime, fbxsdk::FbxPose* pPose = NULL, fbxsdk::FbxAMatrix* pParentGlobalPosition = NULL)
			{
				fbxsdk::FbxAMatrix lGlobalPosition;
				bool        lPositionFound = false;

				if (pPose)
				{
					int lNodeIndex = pPose->Find(pNode);

					if (lNodeIndex > -1)
					{
						// The bind pose is always a global matrix.
						// If we have a rest pose, we need to check if it is
						// stored in global or local space.
						if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
						{
							lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
						}
						else
						{
							// We have a local matrix, we need to convert it to
							// a global space matrix.
							fbxsdk::FbxAMatrix lParentGlobalPosition;

							if (pParentGlobalPosition)
							{
								lParentGlobalPosition = *pParentGlobalPosition;
							}
							else
							{
								if (pNode->GetParent())
								{
									lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
								}
							}

							fbxsdk::FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
							lGlobalPosition = lParentGlobalPosition * lLocalPosition;
						}

						lPositionFound = true;
					}
				}

				if (!lPositionFound)
				{
					// There is no pose entry for that node, get the current global position instead.

					// Ideally this would use parent global position and local position to compute the global position.
					// Unfortunately the equation 
					//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
					// does not hold when inheritance type is other than "Parent" (RSrs).
					// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
					lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
				}

				return lGlobalPosition;
			}

			fbxsdk::FbxAMatrix GetGeometryTransformation(fbxsdk::FbxNode* inNode)
			{
				if (!inNode)
				{
					throw std::exception("Null for mesh geometry");
				}

				const fbxsdk::FbxVector4 lT = inNode->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
				const fbxsdk::FbxVector4 lR = inNode->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
				const fbxsdk::FbxVector4 lS = inNode->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);

				return fbxsdk::FbxAMatrix(lT, lR, lS);
			}

			void CFBXRendererFBX_SDK::Draw(nsDx12Wrappers::CCommandList* commandList)
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

					commandList->DrawInstanced(m_numIndicesArray.at(i));

				}
				return;
			}

			CFBXRendererFBX_SDK::CFBXRendererFBX_SDK(const SModelInitData& modelInitData)
			{
				Init(modelInitData);
				return;
			}
			CFBXRendererFBX_SDK::~CFBXRendererFBX_SDK()
			{
				Terminate();
				return;
			}

			void CFBXRendererFBX_SDK::UpdateWorldMatrix(
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

			void CFBXRendererFBX_SDK::UpdateAnimation(float deltaTime)
			{

				return;
			}

			void CFBXRendererFBX_SDK::Terminate()
			{
				Release();
				return;
			}

			void CFBXRendererFBX_SDK::Release()
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

			bool CFBXRendererFBX_SDK::Init(const SModelInitData& modelInitData)
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

				geometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);
				unsigned int totalFrames = 0;
				
				m_animTimeInfo.Init(fbxScene);


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
				m_numIndicesArray.resize(meshObjectCount);
				m_weightTableArray.resize(meshObjectCount);
				//m_boneMatrixArray.resize(meshObjectCount);
				m_nodeMatrixByFrameArray.resize(meshObjectCount);
				m_materialDHs.resize(meshObjectCount);
				for (int i = 0; i < meshObjectCount; i++)
				{
					CreateMesh(
						static_cast<unsigned int>(i),
						fbxScene->GetSrcObject<fbxsdk::FbxMesh>(i),
						&verticesArray,
						&indicesArray,
						modelInitData
					);
				}

				for (int boneIdx = 0; boneIdx < m_boneInfoArray.size(); boneIdx++)
				{
					auto initMat = m_boneInfoArray.at(boneIdx).initMatrix;
					auto globalBindposeInvMat = m_boneInfoArray.at(boneIdx).globalBindposeInvMatrix;
					auto invInitMat = initMat;
					invInitMat.Inverse();
					auto frameMat = m_boneInfoArray.at(boneIdx).frameMatrixArray.at(30);
					//frameMat.MakeRotationY(3.14f * 0.5f);
					//frameMat = nsMath::CMatrix::Identity();
					auto matTmp = frameMat;

					nsMath::CMatrix mTrans;
					mTrans.m_fMat[3][0] = matTmp.m_fMat[3][0];
					mTrans.m_fMat[3][1] = matTmp.m_fMat[3][1];
					mTrans.m_fMat[3][2] = matTmp.m_fMat[3][2];

					matTmp.m_vec4Mat[3] = nsMath::CVector4::Identity();

					nsMath::CVector3 scale;
					nsMath::CMatrix mScale;
					// 行列から拡大率を取得する。
					scale.x = matTmp.m_vec4Mat[0].Length();
					scale.y = matTmp.m_vec4Mat[1].Length();
					scale.z = matTmp.m_vec4Mat[2].Length();
					mScale.MakeScaling(scale);

					// 行列から拡大率と平行移動量を除去して回転量を取得する。
					matTmp.m_vec4Mat[0].Normalize();
					matTmp.m_vec4Mat[1].Normalize();
					matTmp.m_vec4Mat[2].Normalize();
					nsMath::CMatrix mRot = matTmp;
					//if (modelInitData.isVertesTranspos)
					//{
					//	mRot.Transpose();
					//}
					//nsMath::CMatrix mBias;
					//m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

					//if (boneIdx == m_boneNameTable.at("J_Bip_L_UpperArm"))
					{
						nsMath::CMatrix mWorld = mScale * mRot * mTrans;
						m_boneMatrixArray.at(boneIdx) = invInitMat * frameMat;
						m_boneMatrixArray.at(boneIdx) = globalBindposeInvMat * frameMat;
					}
					//m_boneMatrixArray.at(boneIdx) = nsMath::CMatrix::Identity();

				}
				

				CreateVertexAndIndexBuffer(verticesArray, indicesArray);



				// Sceneからデータを取得したため、Sceneはもういらない。
				DestroySdkObjects(fbxManager, true);

				CreateConstantBufferView();

				CreateShaderResourceView();

				return true;
			}


			bool CFBXRendererFBX_SDK::InitializeSdkObjects(
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

			void CFBXRendererFBX_SDK::DestroySdkObjects(fbxsdk::FbxManager* pManager, bool exitStatus)
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

			void CFBXRendererFBX_SDK::LoadMaterial(
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


			void CFBXRendererFBX_SDK::CreateMesh(
				unsigned int objectIdx,
				const fbxsdk::FbxMesh* mesh,
				std::vector<std::vector<SFbxVertex>>* pVerticesArray,
				std::vector<std::vector<unsigned short>>* pIndicesArray,
				const SModelInitData& modelInitData
			)
			{
				auto* pVertices = &pVerticesArray->at(objectIdx);
				auto* pIndices = &pIndicesArray->at(objectIdx);

				// メッシュの頂点座標
				const auto* meshVertices = mesh->GetControlPoints();
				// メッシュの頂点数
				const int meshVerticesCount = mesh->GetControlPointsCount();
				// メッシュのインデックスバッファ
				const int* meshIndices = mesh->GetPolygonVertices();
				// メッシュのインデックスバッファ数
				const int meshIndicesCount = mesh->GetPolygonVertexCount();
				// メッシュのポリゴン数
				const auto meshPolygonCount = mesh->GetPolygonCount();


				// fbxのワールド行列。
				const auto& meshMWorld = mesh->GetNode()->EvaluateGlobalTransform();

				m_weightTableArray.at(objectIdx).resize(meshVerticesCount);

				// ワールド行列を使いやすいように、このエンジンの行列クラスにコピーする。
				nsMath::CMatrix fbxMWolrd;
				for (int y = 0; y < 4; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						fbxMWolrd.m_fMat[y][x] = static_cast<float>(meshMWorld[y][x]);
					}
				}



				//// 単位をそろえるため、平行移動量を100.0fで割る。
				//fbxMWolrd.m_fMat[3][0] /= 100.0f;
				//fbxMWolrd.m_fMat[3][1] /= 100.0f;
				//fbxMWolrd.m_fMat[3][2] /= 100.0f;
				
				nsMath::CMatrix mTrans;
				mTrans.m_fMat[3][0] = fbxMWolrd.m_fMat[3][0];
				mTrans.m_fMat[3][1] = fbxMWolrd.m_fMat[3][1];
				mTrans.m_fMat[3][2] = fbxMWolrd.m_fMat[3][2];

				fbxMWolrd.m_vec4Mat[3] = nsMath::CVector4::Identity();

				nsMath::CVector3 scale;
				nsMath::CMatrix mScale;
				// 行列から拡大率を取得する。
				scale.x = fbxMWolrd.m_vec4Mat[0].Length();
				scale.y = fbxMWolrd.m_vec4Mat[1].Length();
				scale.z = fbxMWolrd.m_vec4Mat[2].Length();
				//scale.Scale(0.01f);
				mScale.MakeScaling(scale);

				// 行列から拡大率と平行移動量を除去して回転量を取得する。
				fbxMWolrd.m_vec4Mat[0].Normalize();
				fbxMWolrd.m_vec4Mat[1].Normalize();
				fbxMWolrd.m_vec4Mat[2].Normalize();
				nsMath::CMatrix mRot = fbxMWolrd;
				//if (modelInitData.isVertesTranspos)
				//{
				//	mRot.Transpose();
				//}
				//nsMath::CMatrix mBias;
				m_bias.MakeRotationFromQuaternion(modelInitData.vertexBias);

				nsMath::CMatrix mWorld = m_bias * mScale * mRot * mTrans;


				// 〇頂点バッファのデータをコピーする。
				pVertices->resize(meshVerticesCount);
				const auto& node = mesh->GetNode();
				const auto& lclTranslation = node->LclTranslation.Get();
				const auto& lclSclaing = node->LclScaling.Get();
				const auto& lclRotation = node->LclRotation.Get();

				//・座標のコピー。
				for (int vertexIdx = 0; vertexIdx < meshVerticesCount; vertexIdx++)
				{
					// 座標をコピー
					for (int i = 0; i < 3; i++)
					{
						pVertices->at(vertexIdx).position.m_fVec[i] = 
							static_cast<float>(meshVertices[vertexIdx][i]);
					}
					mWorld.Apply(pVertices->at(vertexIdx).position);
					
					//unsigned short index = static_cast<unsigned short>(kFbxIndices[i]);

					//// 座標をコピー
					//// ・右手系(Fbx)から左手系(DirectX)に変換するため、X軸を反転。
					//// ・Zup(Fbx)からYup(DirectX)に変換するため、
					//// 　Y = fbxZ
					//// 　Z = -fbxY
					//// 　を入れる。 

					//pVertices->at(i).position.x =
					//	static_cast<float>(-kFbxVertices[index][0]);
					//pVertices->at(i).position.y =
					//	static_cast<float>(kFbxVertices[index][2]);
					//pVertices->at(i).position.z =
					//	static_cast<float>(-kFbxVertices[index][1]);

					//mWorld.Apply(pVertices->at(i).position);
				}

				mWorld.Inverse();
				mWorld.Transpose();

				const auto* elementNormal = mesh->GetElementNormal();


				if (elementNormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
				//if (true)
				{
					// 制御点によるマッピングモード。
					// メッシュは滑らかで柔らかくなければなりません。
					// 各制御点を取得することで、法線を得ることができます。

					
					// 法線要素のマッピングモードは制御点によるものなので、各頂点の法線を取得しましょう。
					for (int vertexIdx = 0; vertexIdx < meshVerticesCount; vertexIdx++)
					{
						int normalIdx = 0;

						// 法線の参照モードによって法線のインデックスの取得方法が変わります。

						if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
						{
							// 参照モードがDirectの場合、法線のインデックスは頂点のインデックスと同じです。
							// 制御頂点のインデックスで法線を取得します。
							normalIdx = vertexIdx;
						}
						else if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eIndexToDirect)
						{
							// 参照モードがindex-to-directの場合、index-to-directの値で法線を取得します。
							normalIdx = elementNormal->GetIndexArray().GetAt(vertexIdx);
						}

						// 各頂点の法線を取得します。
						const auto& normal = elementNormal->GetDirectArray().GetAt(normalIdx);

						// 法線をコピー。
						for (int i = 0; i < 3; i++)
						{
							pVertices->at(vertexIdx).normal.m_fVec[i] = 
								static_cast<float>(normal[i]);

						}
						mWorld.Apply(pVertices->at(vertexIdx).normal);
						pVertices->at(vertexIdx).normal.Normalize();
					}
				}
				// 未実装
				else if (elementNormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
				{
					// ポリゴン-頂点によるマッピングモード。
					// ポリゴン-頂点を取得することで法線を得ることができます。

					int polygonVertexIdx = 0;

					// 法線要素のマッピングモードがポリゴン-頂点であるため、各ポリゴンの法線を取得しましょう。
					for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
					{
						// ポリゴンサイズを取得すると、現在のポリゴンの頂点の数が分かります。
						int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);
						// 現在のポリゴンの各頂点を取得する。
						for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
						{
							int normalIdx = 0;
							int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);
							// 法線の参照モードによって法線のインデックスの取得方法が変わります。

							if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
							{
								// 参照モードはDirectの場合、法線のインデックスはpolygonVertexIdxと同じです。
								normalIdx = polygonVertexIdx;
							}
							else if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eIndexToDirect)
							{
								// 参照モードがindex-to-directの場合、法線はindex-to-directの値で取得される。
								normalIdx = elementNormal->GetIndexArray().GetAt(polygonVertexIdx);
							}

							// 各ポリゴン頂点の法線を取得する。
							const auto& normal = elementNormal->GetDirectArray().GetAt(normalIdx);

							// 法線をコピー。
							// しなきゃいけないけど、未実装。
							//nsGameWindow::MessageBoxWarning(L"法線のマッピングモードがeByPolygonVertexですが、未対応です。");

							// 法線をコピー。
							if (pVertices->at(polygonVertexIndex).normal.LengthSq() > FLT_EPSILON)
							{
								nsMath::CVector3 newNormal;
								for (int i = 0; i < 3; i++)
								{
									newNormal.m_fVec[i] = static_cast<float>(normal[i]);
								}
								mWorld.Apply(newNormal);
								newNormal.Normalize();
								pVertices->at(polygonVertexIndex).normal.Lerp(
									0.5f,
									pVertices->at(polygonVertexIndex).normal,
									newNormal
								);
								pVertices->at(polygonVertexIndex).normal.Normalize();
							}
							else
							{
								for (int i = 0; i < 3; i++)
								{
									pVertices->at(polygonVertexIndex).normal.m_fVec[i] =
										static_cast<float>(normal[i]);

								}
								mWorld.Apply(pVertices->at(polygonVertexIndex).normal);
								pVertices->at(polygonVertexIndex).normal.Normalize();
							}



							polygonVertexIdx++;

						}
					}
				}



				// ・法線のコピー。
				//fbxsdk::FbxArray<fbxsdk::FbxVector4> fbxNormals;
				//mesh->GetPolygonVertexNormals(fbxNormals);
				//// 法線は頂点とは違い、インデックスバッファの順番で作られているので
				//// そのままの順番でコピーする。

				//for (int i = 0; i < fbxNormals.Size(); i++)
				//{
				//	// 法線をコピー
				//	// 右手系(Fbx)から左手系(DirectX)に変換するため、X軸を反転。
				//	// ・Zup(Fbx)からYup(DirectX)に変換するため、
				//	// 　Y = fbxZ
				//	// 　Z = -fbxY
				//	// 　を入れる。 
				//	pVertices->at(i).normal.x =
				//		-static_cast<float>(fbxNormals[i][0]);
				//	pVertices->at(i).normal.y =
				//		static_cast<float>(fbxNormals[i][2]);
				//	pVertices->at(i).normal.z =
				//		static_cast<float>(-fbxNormals[i][1]);

				//	mWorld.Apply(pVertices->at(i).normal);
				//}

				// ・頂点カラーのコピー。
				const fbxsdk::FbxGeometryElementVertexColor* vertexColor = nullptr;
				if (mesh->GetElementVertexColorCount() > 0)
				{
					//vertexColor = mesh->GetElementVertexColor();
				}

				if (vertexColor != nullptr)
				{
					if (vertexColor->GetMappingMode() == fbxsdk::FbxLayerElement::eByPolygonVertex &&
						vertexColor->GetReferenceMode() == fbxsdk::FbxLayerElement::eIndexToDirect)
					{
						const auto& vcArray = vertexColor->GetDirectArray();
						
						const auto& vcIndices = vertexColor->GetIndexArray();

						const int vcIndicesCount = vcIndices.GetCount();
						const int hoge = vcArray.GetCount();
						//for (int i = 0; i < vcIndicesCount; i++)
						for (int i = 0; i < vcArray.GetCount(); i++)
						{
							//int id = vcIndices.GetAt(i);
							//const auto& color = vcArray.GetAt(id);
							const auto& color = vcArray.GetAt(i);

							pVertices->at(i).color.r = static_cast<float>(color.mRed);
							pVertices->at(i).color.g = static_cast<float>(color.mGreen);
							pVertices->at(i).color.b = static_cast<float>(color.mBlue);
							pVertices->at(i).color.a = static_cast<float>(color.mAlpha);
						}
					}

				}

				{
					fbxsdk::FbxStringList uvSetNameList;
					mesh->GetUVSetNames(uvSetNameList);
					int uvSetNamesCount = uvSetNameList.GetCount();
					if (uvSetNamesCount > 1)
					{
						int a = 1;
					}
					// マルチテクスチャには対応しないので最初の名前を使う
					const auto& uvSetName = uvSetNameList.GetStringAt(0);
					const auto& elementUV = mesh->GetElementUV(uvSetName);

					// eByPolygonVertexとeByControlPointのマッピングモードのみ対応。

					// uvデータを参照するインデックスの配列
					const bool userIdx =
						elementUV->GetReferenceMode() != fbxsdk::FbxGeometryElement::eDirect;
					const int indexCount =
						(userIdx) ? elementNormal->GetIndexArray().GetCount() : 0;


					if (elementUV->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
					//if (true)
					{
						// ポリゴン単位でデータを反復処理する。
						for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
						{
							// ポリゴンサイズを取得すると、現在のポリゴンの頂点の数が分かります。
							int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);

							for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
							{
								// 制御点配列から現在の頂点のインデックスを取得します。
								int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);

								// UV インデックスは参照モードに依存する
								int uvIndex = userIdx ? 
									elementUV->GetIndexArray().GetAt(polygonVertexIndex) : polygonVertexIndex;

								const auto& uvValue = elementUV->GetDirectArray().GetAt(uvIndex);

								// uvのコピー。
								for (int i = 0; i < 2; i++)
								{
									pVertices->at(polygonVertexIndex).uv.m_fVec[i] = 
										static_cast<float>(uvValue[i]);
								}

								pVertices->at(polygonVertexIndex).uv.y =
									1.0f - pVertices->at(polygonVertexIndex).uv.y;
							}
						}
					}
					// @todo 未実装
					else if (elementUV->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
					{
						int polygonVertexIdx = 0;
						// ポリゴン単位でデータを反復処理する。
						for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
						{
							// ポリゴンサイズを取得すると、現在のポリゴンの頂点の数が分かります。
							int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);
							for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
							{
								// 制御点配列から現在の頂点のインデックスを取得します。
								int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);

								// UV インデックスは参照モードに依存する
								int uvIndex = userIdx ?
									elementUV->GetIndexArray().GetAt(polygonVertexIdx) : polygonVertexIdx;

								const auto& uvValue = elementUV->GetDirectArray().GetAt(uvIndex);

								// UVをコピー。
								// しなきゃいけないけど、未実装。
								//nsGameWindow::MessageBoxWarning(L"UVのマッピングモードがeByPolygonVertexですが、未対応です。");

								// uvのコピー。
								if (pVertices->at(polygonVertexIndex).uv.LengthSq() > FLT_EPSILON)
								{
									int a = 1;
								}
								for (int i = 0; i < 2; i++)
								{
									pVertices->at(polygonVertexIndex).uv.m_fVec[i] =
										static_cast<float>(uvValue[i]);
								}
								pVertices->at(polygonVertexIndex).uv.y = 
									1.0f - pVertices->at(polygonVertexIndex).uv.y;
								polygonVertexIdx++;
							}

						}
					}

				}

				//// ・UVのコピー。
				//fbxsdk::FbxStringList uvSetNames;
				//mesh->GetUVSetNames(uvSetNames);

				//fbxsdk::FbxArray<FbxVector2> uvBuffer;
				//// UVSetの名前からUVSetを取得する
				//// 今回はマルチテクスチャには対応しないので最初の名前を使う
				//mesh->GetPolygonVertexUVs(uvSetNames.GetStringAt(0), uvBuffer);

				//for (int i = 0; i < uvBuffer.Size(); i++)
				//{
				//	const auto& uv = uvBuffer[i];

				//	pVertices->at(i).uv.x = static_cast<float>(uv[0]);
				//	// Vは反転させる
				//	pVertices->at(i).uv.y = (1.0f - static_cast<float>(uv[1]));
				//	//pVertices->at(i).uv.y = static_cast<float>(uv[1]);
				//}

				// インデックスバッファのコピー。
				pIndices->resize(meshIndicesCount);
				for (int indicesIdx = 0; indicesIdx < meshIndicesCount; indicesIdx++)
				{
					pIndices->at(indicesIdx) = meshIndices[indicesIdx];
				}
				//// 〇インデックスバッファを作り直す。

				//// fbxのポリゴン数
				//const auto kFbxPolygonCount = mesh->GetPolygonCount();
				//// インデックスバッファの数は、ポリゴン数 * 1ポリゴンの頂点数(3)
				//pIndices->resize(kFbxPolygonCount * 3);

				//for (int i = 0; i < kFbxPolygonCount; i++)
				//{
				//	// インデックスバッファの作成。
				//	// 右手系(Fbx)から左手系(DirectX)に変換するため、逆回りにする。
				//	pIndices->at(i * 3) = i * 3 + 2;
				//	pIndices->at(i * 3 + 1) = i * 3 + 1;
				//	pIndices->at(i * 3 + 2) = i * 3;
				//}


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
					auto skinType = skin->GetSkinningType();
					const int clusterCount = skin->GetClusterCount();
					for (int clusterIdx = 0; clusterIdx < clusterCount; clusterIdx++)
					{
						// クラスタ(ボーン)
						auto cluster = skin->GetCluster(clusterIdx);
						auto linkMode = cluster->GetLinkMode();
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

						if (m_boneNameTable.count(clusterName) == 0)
						{
							m_boneNameTable.emplace(clusterName, clusterIdx);
						}
						else
						{
							int a = 1;
						}

					}
				}

				for (int i = 0; i < meshVerticesCount; i++)
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


				int maxFrameCount = m_animTimeInfo.GetMaxFrame();
				m_nodeMatrixByFrameArray.at(objectIdx).resize(maxFrameCount);
				for (int frameIdx = 0; frameIdx < maxFrameCount; frameIdx++)
				{
					const auto node = mesh->GetNode();
					const auto& mat = node->EvaluateGlobalTransform(m_animTimeInfo.GetTime(frameIdx));
					for (int y = 0; y < 4; y++)
					{
						for (int x = 0; x < 4; x++)
						{
							m_nodeMatrixByFrameArray.at(objectIdx).at(frameIdx).m_fMat[y][x] =
								static_cast<float>(mat[y][x]);
						}
					}
				}

				if (m_boneInfoArray.empty())
				{
					auto meshNode = mesh->GetNode();
					fbxsdk::FbxAMatrix geometryTransform = GetGeometryTransformation(meshNode);

					for (int skinIdx = 0; skinIdx < skinCount; skinIdx++)
					{
						fbxsdk::FbxSkin* skin = static_cast<fbxsdk::FbxSkin*>(
							mesh->GetDeformer(skinIdx, fbxsdk::FbxDeformer::EDeformerType::eSkin));
						const int clusterCount = skin->GetClusterCount();

						m_boneInfoArray.resize(clusterCount);
						m_boneMatrixArray.resize(clusterCount);
						for (int clusterIdx = 0; clusterIdx < clusterCount; clusterIdx++)
						{
							// クラスタ(ボーン)
							auto cluster = skin->GetCluster(clusterIdx);
							m_boneInfoArray.at(clusterIdx).name = cluster->GetLink()->GetName();
							m_boneInfoArray.at(clusterIdx).frameNum = m_animTimeInfo.GetMaxFrame();
							fbxsdk::FbxAMatrix initMat;
							cluster->GetTransformLinkMatrix(initMat);

							fbxsdk::FbxAMatrix transformMatrix, transformLinkMatrix,
								globalBindposeInvMatrix;
							cluster->GetTransformMatrix(transformMatrix);

							// バインディング時のメッシュの変形
							cluster->GetTransformLinkMatrix(transformLinkMatrix);

							// バインディング時のクラスタの、クラスタ空間からワールド空間への変換
							globalBindposeInvMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

							for (int y = 0; y < 4; y++)
							{
								for (int x = 0; x < 4; x++)
								{
									m_boneInfoArray.at(clusterIdx).globalBindposeInvMatrix.m_fMat[y][x] =
										static_cast<float>(globalBindposeInvMatrix[y][x]);
									m_boneInfoArray.at(clusterIdx).initMatrix.m_fMat[y][x] =
										static_cast<float>(initMat[y][x]);
								}
							}

							m_boneInfoArray.at(clusterIdx).frameMatrixArray.resize(m_boneInfoArray.at(clusterIdx).frameNum);
							for (unsigned int frameIdx = 0; frameIdx < m_boneInfoArray.at(clusterIdx).frameNum; frameIdx++)
							{
								const auto& time = m_animTimeInfo.GetTime(frameIdx);
								fbxsdk::FbxAMatrix currentTransformOffset = meshNode->EvaluateGlobalTransform(time) * geometryTransform;
								currentTransformOffset.Inverse();
								const auto& frameMat = currentTransformOffset.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(time);
								for (int y = 0; y < 4; y++)
								{
									for (int x = 0; x < 4; x++)
									{
										m_boneInfoArray.at(clusterIdx).frameMatrixArray.at(frameIdx).m_fMat[y][x] = 
											static_cast<float>(frameMat[y][x]);
									}
								}
							}

						}
					}
				}
				return;
			}

			bool CFBXRendererFBX_SDK::CreateVertexAndIndexBuffer(
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
					const auto& vertices = verticesArray.at(i);
					auto resV = m_vertexBuffers.at(i)->Init(
						static_cast<unsigned int>(alignedStrideSize * vertices.size()),
						alignedStrideSize,
						&vertices.at(0)
					);

					m_indexBuffers.at(i) = new nsDx12Wrappers::CIndexBuffer();
					const auto& indices = indicesArray.at(i);
					m_numIndicesArray.at(i) = static_cast<unsigned int>(indices.size());
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

			bool CFBXRendererFBX_SDK::CreateConstantBufferView()
			{
				// 〇定数バッファ作成
				auto cbSize = sizeof(nsMath::CMatrix) * 2;

				unsigned int boneNum = static_cast<unsigned int>(m_boneMatrixArray.size());
				cbSize += sizeof(nsMath::CMatrix) * boneNum;

				m_constantBuffer.Init(static_cast<unsigned int>(cbSize), L"FBXModel");

				nsMath::CMatrix mWorld = nsMath::CMatrix::Identity();

				auto mappedCB =
					static_cast<nsMath::CMatrix*>(m_constantBuffer.GetMappedConstantBuffer());
				mappedCB[0] = mWorld;
				auto mViewProj = CGraphicsEngine::GetInstance()->GetMatrixViewProj();
				mappedCB[1] = mWorld * mViewProj;

				copy(m_boneMatrixArray.begin(), m_boneMatrixArray.end(), mappedCB + 2);

				// 〇ディスクリプタヒープ作成
				constexpr unsigned int numDescHeaps = 1;
				m_descriptorHeap.InitAsCbvSrvUav(numDescHeaps, L"FBXModel");

				// 〇定数バッファビュー作成
				m_constantBuffer.CreateConstantBufferView(m_descriptorHeap.GetCPUHandle());




				return true;
			}

			bool CFBXRendererFBX_SDK::CreateMaterialCBVTable(
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

			bool CFBXRendererFBX_SDK::CreateShaderResourceView()
			{


				return true;
			}


			
		}
	}
}