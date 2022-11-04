#include "ModelRenderer.h"
#include "../Utils/StringManipulation.h"
#include "../GameWindow/MessageBox.h"
#include "../PMDModels/PMDRenderer.h"
#include "../FBXModels/FBXRenderer.h"
#include "../GraphicsEngine.h"

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
				m_renderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);

				return true;
			}

			void CModelRenderer::Update(float deltaTime)
			{
				m_renderer->UpdateWorldMatrix(m_position, m_rotation, m_scale);

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

				return;
			}

			void CModelRenderer::Terminate()
			{
				if (m_rendererType != CRendererTable::EnRendererType::enNone)
				{
					DisableDrawing();
				}
				if (m_renderer)
				{
					delete m_renderer;
				}
				return;
			}

			void CModelRenderer::EnableDrawing()
			{
				if (IsEnableDrawing())
				{
					return;
				}

				CGraphicsEngine::GetInstance()->GetRendererTable()->RegistRenderer(
					m_rendererType, m_renderer);
				m_isEnableDrawing = true;
				return;
			}

			void CModelRenderer::DisableDrawing()
			{
				if (IsEnableDrawing() != true)
				{
					return;
				}

				CGraphicsEngine::GetInstance()->GetRendererTable()->RemoveRenderer(
					m_rendererType, m_renderer);
				m_isEnableDrawing = false;
				return;
			}

			void CModelRenderer::CreateRenderer(
				EnModelFormat* pModelFormat, const SModelInitData& modelInitData)
			{
				std::string msg;

				*pModelFormat = modelInitData.modelFormat;

				if (*pModelFormat == EnModelFormat::enNone)
				{
					// モデルのフォーマットが指定されていなかったら、ファイルパスから調べる。
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
					m_renderer = new nsFBXModels::CFBXRenderer(modelInitData.modelFilePath);
					break;
				case EnModelFormat::enVRM:
					msg = "モデルのロードに失敗しました。\nごめんなさい、この拡張子はまだ対応していません。\n";
					msg += modelInitData.modelFilePath;
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					break;
				default:
					msg = "モデルのロードに失敗しました。\nファイルパスが間違っている、または、拡張子が対応していません。\n";
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

				m_rendererType = modelInitData.rendererType;

				if (m_rendererType == CRendererTable::EnRendererType::enNone)
				{
					switch (modelFormat)
					{
						case EnModelFormat::enPMD:
							m_rendererType = CRendererTable::EnRendererType::enPMDModel;
							break;
						case EnModelFormat::enFBX:
							m_rendererType = CRendererTable::EnRendererType::enFBXModel;
							break;
						case EnModelFormat::enVRM:
							m_rendererType = CRendererTable::EnRendererType::enNone;

							msg = "レンダラーの登録に失敗しました。\nごめんなさい、このレンダラーはまだ対応していません。\n";
							msg += modelInitData.modelFilePath;
							nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
							break;
						default:
							m_rendererType = CRendererTable::EnRendererType::enNone;

							msg = "レンダラーの登録に失敗しました。\nごめんなさい、このレンダラーはまだ対応していません。\n";
							msg += modelInitData.modelFilePath;
							nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
							break;
					}
				}

				if (m_rendererType != CRendererTable::EnRendererType::enNone)
				{
					EnableDrawing();
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
		}
	}
}