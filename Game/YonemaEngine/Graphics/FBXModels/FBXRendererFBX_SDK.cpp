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
				// ���萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v���Z�b�g
				ID3D12DescriptorHeap* modelDescHeaps[] = { m_descriptorHeap.Get() };
				commandList->SetDescriptorHeaps(1, modelDescHeaps);
				auto descriptorHeapH = m_descriptorHeap.GetGPUHandle();
				commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapH);


				// �}�e���A�����Ƃɕ`��
				// �ŏ��Ƀ}�e���A�����ƂɃ��b�V���������Ă��邽�߁A���b�V�����Ƃɕ`��Ɠ��ӁB
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
				// ���[���h�s��쐬�B
				nsMath::CMatrix mTrans, mRot, mScale, mWorld;
				mTrans.MakeTranslation(position);
				mRot.MakeRotationFromQuaternion(rotation);
				mScale.MakeScaling(scale);
				mWorld = mScale * mRot * mTrans;

				// �萔�o�b�t�@�ɃR�s�[�B
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
				// �t�@�C���Ɏw�肳�ꂽ�C���|�[�g�i���[�_�[�j�t�@�C���`�������o����
				if (fbxManager->GetIOPluginRegistry()->
					DetectReaderFileFormat(modelInitData.modelFilePath, fileFormat) != true)
				{
					// ���o�ł��Ȃ��t�@�C���`���ł��B FbxImporter::eFBX_BINARY �`���Ńg���C���܂��B
					fileFormat = fbxManager->GetIOPluginRegistry()->
						FindReaderIDByDescription("FBX binary (*.fbx)");
				}

				// Fbx�t�@�C���̏������iFbx�t�@�C�����J���j
				if (fbxImporter->Initialize(
					modelInitData.modelFilePath, fileFormat, fbxManager->GetIOSettings()) != true)
				{
					DestroySdkObjects(fbxManager, false);
					return false;
				}


				// Importer�������Ă�f�[�^�𕪉�����Scene�ɓn���B
				// @attention �R�X�g�̍��������I
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

				// ���������`�F�b�N
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

				// �K�v�ɉ����Ď��ϊ����s���B

				// Fbx�t�@�C���ɐݒ肵�Ă��鎲�ݒ�
				fbxsdk::FbxAxisSystem SceneAxisSystem = 
					fbxScene->GetGlobalSettings().GetAxisSystem();
				// ���̃Q�[���̎��ݒ�
				fbxsdk::FbxAxisSystem OurAxisSystem(fbxsdk::FbxAxisSystem::DirectX);
				//fbxsdk::FbxAxisSystem OurAxisSystem(
				//	fbxsdk::FbxAxisSystem::eYAxis,
				//	fbxsdk::FbxAxisSystem::eParityOdd,
				//	fbxsdk::FbxAxisSystem::eRightHanded);

				if (SceneAxisSystem != OurAxisSystem)
				{
					// ���ݒ�ɍ��ق�����Ύ��ϊ����s���B
					OurAxisSystem.ConvertScene(fbxScene);
				}

				// �K�v�ɉ����ĒP�ʕϊ����s���B

				// Fbx�t�@�C���ɐݒ肵�Ă���P�ʐݒ�
				fbxsdk::FbxSystemUnit SceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
				// �Z���`���[�g���ɑ΂��鑊�ΓI�Ȕ{���𒲂ׂ�B
				if (SceneSystemUnit.GetScaleFactor() != 1.0)
				{
					// �ړI�̒P�ʐݒ�łȂ������Ȃ�A�P�ʕϊ����s���B
					fbxsdk::FbxSystemUnit::cm.ConvertScene(fbxScene);
				}

				// ���b�V���ANURBS�A�p�b�`���A�O�p���b�V���ɕϊ�����B
				// �R�X�g�̍��������B
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


				// �C���|�[�g�������߁AImporter�͂�������Ȃ��B
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
					// �s�񂩂�g�嗦���擾����B
					scale.x = matTmp.m_vec4Mat[0].Length();
					scale.y = matTmp.m_vec4Mat[1].Length();
					scale.z = matTmp.m_vec4Mat[2].Length();
					mScale.MakeScaling(scale);

					// �s�񂩂�g�嗦�ƕ��s�ړ��ʂ��������ĉ�]�ʂ��擾����B
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



				// Scene����f�[�^���擾�������߁AScene�͂�������Ȃ��B
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
					nsGameWindow::MessageBoxWarning(L"FbxManager�������ł��܂���ł����B");
					return false;
				}

				// fbx�̃o�[�W������\��
				std::string str("Autodesk FBX SDK version ");
				str += pManager->GetVersion();
				str += "\n";
				::OutputDebugStringA(str.c_str());

				fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(pManager, IOSROOT);
				pManager->SetIOSettings(ios);

				// ���s�f�B���N�g������v���O�C����ǂݍ��ށi�C�Ӂj
				fbxsdk::FbxString lPath = fbxsdk::FbxGetApplicationDirectory();
				pManager->LoadPluginsDirectory(lPath.Buffer());

				pScene = fbxsdk::FbxScene::Create(pManager, "MyFbxScene");
				if (pScene == nullptr)
				{
					::OutputDebugStringA("Error: Unable to create FBX scene!\n");
					nsGameWindow::MessageBoxWarning(L"FbxScene�������ł��܂���ł����B");
					return false;
				}

				return true;
			}

			void CFBXRendererFBX_SDK::DestroySdkObjects(fbxsdk::FbxManager* pManager, bool exitStatus)
			{
				if (pManager)
				{
					// FbxManager���폜���܂��B
					// FbxManager���g�p���Ċ��蓖�Ă�ꂽ�I�u�W�F�N�g�ŁA
					// �����I�ɔj������Ă��Ȃ����̂����ׂĎ����I�ɔj������܂��B
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

					// ���������΍�
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

				// ���b�V���̒��_���W
				const auto* meshVertices = mesh->GetControlPoints();
				// ���b�V���̒��_��
				const int meshVerticesCount = mesh->GetControlPointsCount();
				// ���b�V���̃C���f�b�N�X�o�b�t�@
				const int* meshIndices = mesh->GetPolygonVertices();
				// ���b�V���̃C���f�b�N�X�o�b�t�@��
				const int meshIndicesCount = mesh->GetPolygonVertexCount();
				// ���b�V���̃|���S����
				const auto meshPolygonCount = mesh->GetPolygonCount();


				// fbx�̃��[���h�s��B
				const auto& meshMWorld = mesh->GetNode()->EvaluateGlobalTransform();

				m_weightTableArray.at(objectIdx).resize(meshVerticesCount);

				// ���[���h�s����g���₷���悤�ɁA���̃G���W���̍s��N���X�ɃR�s�[����B
				nsMath::CMatrix fbxMWolrd;
				for (int y = 0; y < 4; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						fbxMWolrd.m_fMat[y][x] = static_cast<float>(meshMWorld[y][x]);
					}
				}



				//// �P�ʂ����낦�邽�߁A���s�ړ��ʂ�100.0f�Ŋ���B
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
				// �s�񂩂�g�嗦���擾����B
				scale.x = fbxMWolrd.m_vec4Mat[0].Length();
				scale.y = fbxMWolrd.m_vec4Mat[1].Length();
				scale.z = fbxMWolrd.m_vec4Mat[2].Length();
				//scale.Scale(0.01f);
				mScale.MakeScaling(scale);

				// �s�񂩂�g�嗦�ƕ��s�ړ��ʂ��������ĉ�]�ʂ��擾����B
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


				// �Z���_�o�b�t�@�̃f�[�^���R�s�[����B
				pVertices->resize(meshVerticesCount);
				const auto& node = mesh->GetNode();
				const auto& lclTranslation = node->LclTranslation.Get();
				const auto& lclSclaing = node->LclScaling.Get();
				const auto& lclRotation = node->LclRotation.Get();

				//�E���W�̃R�s�[�B
				for (int vertexIdx = 0; vertexIdx < meshVerticesCount; vertexIdx++)
				{
					// ���W���R�s�[
					for (int i = 0; i < 3; i++)
					{
						pVertices->at(vertexIdx).position.m_fVec[i] = 
							static_cast<float>(meshVertices[vertexIdx][i]);
					}
					mWorld.Apply(pVertices->at(vertexIdx).position);
					
					//unsigned short index = static_cast<unsigned short>(kFbxIndices[i]);

					//// ���W���R�s�[
					//// �E�E��n(Fbx)���獶��n(DirectX)�ɕϊ����邽�߁AX���𔽓]�B
					//// �EZup(Fbx)����Yup(DirectX)�ɕϊ����邽�߁A
					//// �@Y = fbxZ
					//// �@Z = -fbxY
					//// �@������B 

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
					// ����_�ɂ��}�b�s���O���[�h�B
					// ���b�V���͊��炩�ŏ_�炩���Ȃ���΂Ȃ�܂���B
					// �e����_���擾���邱�ƂŁA�@���𓾂邱�Ƃ��ł��܂��B

					
					// �@���v�f�̃}�b�s���O���[�h�͐���_�ɂ����̂Ȃ̂ŁA�e���_�̖@�����擾���܂��傤�B
					for (int vertexIdx = 0; vertexIdx < meshVerticesCount; vertexIdx++)
					{
						int normalIdx = 0;

						// �@���̎Q�ƃ��[�h�ɂ���Ė@���̃C���f�b�N�X�̎擾���@���ς��܂��B

						if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
						{
							// �Q�ƃ��[�h��Direct�̏ꍇ�A�@���̃C���f�b�N�X�͒��_�̃C���f�b�N�X�Ɠ����ł��B
							// ���䒸�_�̃C���f�b�N�X�Ŗ@�����擾���܂��B
							normalIdx = vertexIdx;
						}
						else if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eIndexToDirect)
						{
							// �Q�ƃ��[�h��index-to-direct�̏ꍇ�Aindex-to-direct�̒l�Ŗ@�����擾���܂��B
							normalIdx = elementNormal->GetIndexArray().GetAt(vertexIdx);
						}

						// �e���_�̖@�����擾���܂��B
						const auto& normal = elementNormal->GetDirectArray().GetAt(normalIdx);

						// �@�����R�s�[�B
						for (int i = 0; i < 3; i++)
						{
							pVertices->at(vertexIdx).normal.m_fVec[i] = 
								static_cast<float>(normal[i]);

						}
						mWorld.Apply(pVertices->at(vertexIdx).normal);
						pVertices->at(vertexIdx).normal.Normalize();
					}
				}
				// ������
				else if (elementNormal->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
				{
					// �|���S��-���_�ɂ��}�b�s���O���[�h�B
					// �|���S��-���_���擾���邱�ƂŖ@���𓾂邱�Ƃ��ł��܂��B

					int polygonVertexIdx = 0;

					// �@���v�f�̃}�b�s���O���[�h���|���S��-���_�ł��邽�߁A�e�|���S���̖@�����擾���܂��傤�B
					for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
					{
						// �|���S���T�C�Y���擾����ƁA���݂̃|���S���̒��_�̐���������܂��B
						int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);
						// ���݂̃|���S���̊e���_���擾����B
						for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
						{
							int normalIdx = 0;
							int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);
							// �@���̎Q�ƃ��[�h�ɂ���Ė@���̃C���f�b�N�X�̎擾���@���ς��܂��B

							if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eDirect)
							{
								// �Q�ƃ��[�h��Direct�̏ꍇ�A�@���̃C���f�b�N�X��polygonVertexIdx�Ɠ����ł��B
								normalIdx = polygonVertexIdx;
							}
							else if (elementNormal->GetReferenceMode() == fbxsdk::FbxGeometryElement::eIndexToDirect)
							{
								// �Q�ƃ��[�h��index-to-direct�̏ꍇ�A�@����index-to-direct�̒l�Ŏ擾�����B
								normalIdx = elementNormal->GetIndexArray().GetAt(polygonVertexIdx);
							}

							// �e�|���S�����_�̖@�����擾����B
							const auto& normal = elementNormal->GetDirectArray().GetAt(normalIdx);

							// �@�����R�s�[�B
							// ���Ȃ��Ⴂ���Ȃ����ǁA�������B
							//nsGameWindow::MessageBoxWarning(L"�@���̃}�b�s���O���[�h��eByPolygonVertex�ł����A���Ή��ł��B");

							// �@�����R�s�[�B
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



				// �E�@���̃R�s�[�B
				//fbxsdk::FbxArray<fbxsdk::FbxVector4> fbxNormals;
				//mesh->GetPolygonVertexNormals(fbxNormals);
				//// �@���͒��_�Ƃ͈Ⴂ�A�C���f�b�N�X�o�b�t�@�̏��Ԃō���Ă���̂�
				//// ���̂܂܂̏��ԂŃR�s�[����B

				//for (int i = 0; i < fbxNormals.Size(); i++)
				//{
				//	// �@�����R�s�[
				//	// �E��n(Fbx)���獶��n(DirectX)�ɕϊ����邽�߁AX���𔽓]�B
				//	// �EZup(Fbx)����Yup(DirectX)�ɕϊ����邽�߁A
				//	// �@Y = fbxZ
				//	// �@Z = -fbxY
				//	// �@������B 
				//	pVertices->at(i).normal.x =
				//		-static_cast<float>(fbxNormals[i][0]);
				//	pVertices->at(i).normal.y =
				//		static_cast<float>(fbxNormals[i][2]);
				//	pVertices->at(i).normal.z =
				//		static_cast<float>(-fbxNormals[i][1]);

				//	mWorld.Apply(pVertices->at(i).normal);
				//}

				// �E���_�J���[�̃R�s�[�B
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
					// �}���`�e�N�X�`���ɂ͑Ή����Ȃ��̂ōŏ��̖��O���g��
					const auto& uvSetName = uvSetNameList.GetStringAt(0);
					const auto& elementUV = mesh->GetElementUV(uvSetName);

					// eByPolygonVertex��eByControlPoint�̃}�b�s���O���[�h�̂ݑΉ��B

					// uv�f�[�^���Q�Ƃ���C���f�b�N�X�̔z��
					const bool userIdx =
						elementUV->GetReferenceMode() != fbxsdk::FbxGeometryElement::eDirect;
					const int indexCount =
						(userIdx) ? elementNormal->GetIndexArray().GetCount() : 0;


					if (elementUV->GetMappingMode() == fbxsdk::FbxGeometryElement::eByControlPoint)
					//if (true)
					{
						// �|���S���P�ʂŃf�[�^�𔽕���������B
						for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
						{
							// �|���S���T�C�Y���擾����ƁA���݂̃|���S���̒��_�̐���������܂��B
							int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);

							for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
							{
								// ����_�z�񂩂猻�݂̒��_�̃C���f�b�N�X���擾���܂��B
								int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);

								// UV �C���f�b�N�X�͎Q�ƃ��[�h�Ɉˑ�����
								int uvIndex = userIdx ? 
									elementUV->GetIndexArray().GetAt(polygonVertexIndex) : polygonVertexIndex;

								const auto& uvValue = elementUV->GetDirectArray().GetAt(uvIndex);

								// uv�̃R�s�[�B
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
					// @todo ������
					else if (elementUV->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
					{
						int polygonVertexIdx = 0;
						// �|���S���P�ʂŃf�[�^�𔽕���������B
						for (int polygonIdx = 0; polygonIdx < meshPolygonCount; polygonIdx++)
						{
							// �|���S���T�C�Y���擾����ƁA���݂̃|���S���̒��_�̐���������܂��B
							int verticesCountInPolygon = mesh->GetPolygonSize(polygonIdx);
							for (int vertInPolyIdx = 0; vertInPolyIdx < verticesCountInPolygon; vertInPolyIdx++)
							{
								// ����_�z�񂩂猻�݂̒��_�̃C���f�b�N�X���擾���܂��B
								int polygonVertexIndex = mesh->GetPolygonVertex(polygonIdx, vertInPolyIdx);

								// UV �C���f�b�N�X�͎Q�ƃ��[�h�Ɉˑ�����
								int uvIndex = userIdx ?
									elementUV->GetIndexArray().GetAt(polygonVertexIdx) : polygonVertexIdx;

								const auto& uvValue = elementUV->GetDirectArray().GetAt(uvIndex);

								// UV���R�s�[�B
								// ���Ȃ��Ⴂ���Ȃ����ǁA�������B
								//nsGameWindow::MessageBoxWarning(L"UV�̃}�b�s���O���[�h��eByPolygonVertex�ł����A���Ή��ł��B");

								// uv�̃R�s�[�B
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

				//// �EUV�̃R�s�[�B
				//fbxsdk::FbxStringList uvSetNames;
				//mesh->GetUVSetNames(uvSetNames);

				//fbxsdk::FbxArray<FbxVector2> uvBuffer;
				//// UVSet�̖��O����UVSet���擾����
				//// ����̓}���`�e�N�X�`���ɂ͑Ή����Ȃ��̂ōŏ��̖��O���g��
				//mesh->GetPolygonVertexUVs(uvSetNames.GetStringAt(0), uvBuffer);

				//for (int i = 0; i < uvBuffer.Size(); i++)
				//{
				//	const auto& uv = uvBuffer[i];

				//	pVertices->at(i).uv.x = static_cast<float>(uv[0]);
				//	// V�͔��]������
				//	pVertices->at(i).uv.y = (1.0f - static_cast<float>(uv[1]));
				//	//pVertices->at(i).uv.y = static_cast<float>(uv[1]);
				//}

				// �C���f�b�N�X�o�b�t�@�̃R�s�[�B
				pIndices->resize(meshIndicesCount);
				for (int indicesIdx = 0; indicesIdx < meshIndicesCount; indicesIdx++)
				{
					pIndices->at(indicesIdx) = meshIndices[indicesIdx];
				}
				//// �Z�C���f�b�N�X�o�b�t�@����蒼���B

				//// fbx�̃|���S����
				//const auto kFbxPolygonCount = mesh->GetPolygonCount();
				//// �C���f�b�N�X�o�b�t�@�̐��́A�|���S���� * 1�|���S���̒��_��(3)
				//pIndices->resize(kFbxPolygonCount * 3);

				//for (int i = 0; i < kFbxPolygonCount; i++)
				//{
				//	// �C���f�b�N�X�o�b�t�@�̍쐬�B
				//	// �E��n(Fbx)���獶��n(DirectX)�ɕϊ����邽�߁A�t���ɂ���B
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
				// �����̃X�P���g���͖��Ή�
				for (int skinIdx = 0; skinIdx < skinCount; skinIdx++)
				{
					fbxsdk::FbxSkin* skin = static_cast<fbxsdk::FbxSkin*>(
						mesh->GetDeformer(skinIdx, fbxsdk::FbxDeformer::EDeformerType::eSkin));
					auto skinType = skin->GetSkinningType();
					const int clusterCount = skin->GetClusterCount();
					for (int clusterIdx = 0; clusterIdx < clusterCount; clusterIdx++)
					{
						// �N���X�^(�{�[��)
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
							// �N���X�^(�{�[��)
							auto cluster = skin->GetCluster(clusterIdx);
							m_boneInfoArray.at(clusterIdx).name = cluster->GetLink()->GetName();
							m_boneInfoArray.at(clusterIdx).frameNum = m_animTimeInfo.GetMaxFrame();
							fbxsdk::FbxAMatrix initMat;
							cluster->GetTransformLinkMatrix(initMat);

							fbxsdk::FbxAMatrix transformMatrix, transformLinkMatrix,
								globalBindposeInvMatrix;
							cluster->GetTransformMatrix(transformMatrix);

							// �o�C���f�B���O���̃��b�V���̕ό`
							cluster->GetTransformLinkMatrix(transformLinkMatrix);

							// �o�C���f�B���O���̃N���X�^�́A�N���X�^��Ԃ��烏�[���h��Ԃւ̕ϊ�
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
				// �Z�萔�o�b�t�@�쐬
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

				// �Z�f�B�X�N���v�^�q�[�v�쐬
				constexpr unsigned int numDescHeaps = 1;
				m_descriptorHeap.InitAsCbvSrvUav(numDescHeaps, L"FBXModel");

				// �Z�萔�o�b�t�@�r���[�쐬
				m_constantBuffer.CreateConstantBufferView(m_descriptorHeap.GetCPUHandle());




				return true;
			}

			bool CFBXRendererFBX_SDK::CreateMaterialCBVTable(
				const std::unordered_map<std::string, SFbxMaterial>& fbxMaterials)
			{
				auto cbSize = static_cast<unsigned int>(sizeof(SFbxMaterial));
				// �萔�o�b�t�@1 + �V�F�[�_�[���\�[�X1 = 2
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
					// �Z�f�B�X�N���v�^�q�[�v�쐬
					m_materialDHTable.emplace(fbxMaterial.first, new nsDx12Wrappers::CDescriptorHeap());
					m_materialDHTable.at(fbxMaterial.first)->InitAsCbvSrvUav(
						numDescHeaps, L"FbxMaterialDH");
					auto matDescHandle = m_materialDHTable.at(fbxMaterial.first)->GetCPUHandle();

					// �Z�萔�o�b�t�@�쐬
					m_materialCBTable.emplace(fbxMaterial.first, new nsDx12Wrappers::CConstantBuffer());
					m_materialCBTable.at(fbxMaterial.first)->Init(
						cbSize, L"FbxMaterialCB", 1, &fbxMaterial.second);
					m_materialCBTable.at(fbxMaterial.first)->Unmap();

					// �Z�萔�o�b�t�@�r���[�쐬
					m_materialCBTable.at(fbxMaterial.first)->CreateConstantBufferView(
						matDescHandle);

					matDescHandle.ptr += inc;

					
					auto& diffuseTexture = m_diffuseTextures.at(fbxMaterial.first);
					// �Z�V�F�[�_�[���\�[�X�r���[�쐬
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

				// �k���}�e���A���쐬
				SFbxMaterial nullFbxMaterial;
				nullFbxMaterial.diffuse = { 0.909f, 0.56f,0.8f, 1.0f };

				// �Z�f�B�X�N���v�^�q�[�v�쐬
				m_materialDHTable.emplace("", new nsDx12Wrappers::CDescriptorHeap());
				m_materialDHTable.at("")->InitAsCbvSrvUav(
					numDescHeaps, L"FbxNullMaterialDH");

				auto matDescHandle = m_materialDHTable.at("")->GetCPUHandle();

				// �Z�萔�o�b�t�@�쐬
				m_materialCBTable.emplace("", new nsDx12Wrappers::CConstantBuffer());
				m_materialCBTable.at("")->Init(
					cbSize, L"FbxNullMaterialCB", 1, &nullFbxMaterial);
				m_materialCBTable.at("")->Unmap();

				// �Z�萔�o�b�t�@�r���[�쐬
				m_materialCBTable.at("")->CreateConstantBufferView(
					matDescHandle);

				matDescHandle.ptr += inc;

				// �Z�V�F�[�_�[���\�[�X�r���[�쐬
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