#include "ModelRenderer.h"
#include "../Utils/StringManipulation.h"
#include "../PMDModels/PMDRenderer.h"
#include "../FBXModels/FBXRendererFBX_SDK.h"
#include "../FBXModels/FBXRendererAssimp.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			const char* CModelRenderer::m_kModelFormatExtensions
				[static_cast<int>(EnModelFormat::enNumModelFormat)] = 
			{
				"pmd",
				"fbx",
				"vrm"
			};

			bool CModelRenderer::Start()
			{
				UpdateWorldMatrix();
				m_renderer->PlayAnimation(0);
				return true;
			}

			void CModelRenderer::Update(float deltaTime)
			{
				UpdateWorldMatrix();

				m_renderer->UpdateAnimation(deltaTime);

				return;
			}

			void CModelRenderer::OnDestroy()
			{
				Terminate();

				return;
			}


			void CModelRenderer::Init(const SModelInitData& modelInitData)
			{

				EnModelFormat modelFormat;

				CreateRenderer(&modelFormat, modelInitData);

				RegistToRendererTable(modelFormat, modelInitData);

				UpdateWorldMatrix();

				return;
			}

			void CModelRenderer::Terminate()
			{
				if (m_renderer)
				{
					m_renderer->DisableDrawing();
					delete m_renderer;
					m_renderer = nullptr;
				}
				return;
			}

			void CModelRenderer::CreateRenderer(
				EnModelFormat* pModelFormat, const SModelInitData& modelInitData)
			{
				std::string msg;

				*pModelFormat = modelInitData.modelFormat;

				if (*pModelFormat == EnModelFormat::enNone)
				{
					// ���f���̃t�H�[�}�b�g���w�肳��Ă��Ȃ�������A�t�@�C���p�X���璲�ׂ�B
					auto extension = nsUtils::GetExtension(modelInitData.modelFilePath);
					*pModelFormat = FindMatchExtension(extension);
				}

				switch (*pModelFormat)
				{
				case EnModelFormat::enPMD:
					m_renderer = new nsPMDModels::CPMDRenderer(
						modelInitData.modelFilePath, modelInitData.animFilePath);
					break;
				case EnModelFormat::enFBX:
					//m_renderer = new nsFBXModels::CFBXRendererFBX_SDK(modelInitData);
					m_renderer = new nsFBXModels::CFBXRendererAssimp(modelInitData);
					break;
				case EnModelFormat::enVRM:
					msg = "���f���̃��[�h�Ɏ��s���܂����B\n���߂�Ȃ����A���̊g���q�͂܂��Ή����Ă��܂���B\n";
					msg += modelInitData.modelFilePath;
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					break;
				default:
					msg = "���f���̃��[�h�Ɏ��s���܂����B\n�t�@�C���p�X���Ԉ���Ă���A�܂��́A�g���q���Ή����Ă��܂���B\n";
					msg += modelInitData.modelFilePath;
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					break;
				}

				return;
			}

			void CModelRenderer::RegistToRendererTable(
				EnModelFormat modelFormat, const SModelInitData& modelInitData)
			{
				std::string msg;

				if (m_renderer == nullptr)
				{
					return;
				}

				m_renderer->SetRenderType(modelInitData.rendererType);

				if (m_renderer->GetRenderType() == CRendererTable::EnRendererType::enNone)
				{
					switch (modelFormat)
					{
						case EnModelFormat::enPMD:
							m_renderer->SetRenderType(CRendererTable::EnRendererType::enPMDModel);
							break;
						case EnModelFormat::enFBX:
							m_renderer->SetRenderType(CRendererTable::EnRendererType::enFBXModel);
							break;
						case EnModelFormat::enVRM:
							m_renderer->SetRenderType(CRendererTable::EnRendererType::enNone);

							msg = "�����_���[�̓o�^�Ɏ��s���܂����B\n���߂�Ȃ����A���̃����_���[�͂܂��Ή����Ă��܂���B\n";
							msg += modelInitData.modelFilePath;
							nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
							break;
						default:
							m_renderer->SetRenderType(CRendererTable::EnRendererType::enNone);

							msg = "�����_���[�̓o�^�Ɏ��s���܂����B\n���߂�Ȃ����A���̃����_���[�͂܂��Ή����Ă��܂���B\n";
							msg += modelInitData.modelFilePath;
							nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
							break;
					}
				}

				if (m_renderer->GetRenderType() != CRendererTable::EnRendererType::enNone)
				{
					m_renderer->EnableDrawing();
				}

				return;
			}

			EnModelFormat CModelRenderer::FindMatchExtension(const char* extension)
			{
				EnModelFormat format = EnModelFormat::enNone;
				if (extension == nullptr)
				{
					return format;
				}

				for (int i = 0; i < static_cast<int>(EnModelFormat::enNumModelFormat); i++)
				{
					if (strcmp(extension, m_kModelFormatExtensions[i]) == 0)
					{
						format = static_cast<EnModelFormat>(i);
						break;
					}
				}
				
				return format;
			}

			void CModelRenderer::UpdateWorldMatrix() noexcept
			{
				if (m_renderer)
				{
					m_renderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);
				}

				return;
			}
		}
	}
}