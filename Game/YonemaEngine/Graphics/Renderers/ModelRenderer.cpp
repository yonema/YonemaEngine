#include "ModelRenderer.h"
#include "../Utils/StringManipulation.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
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


			void CModelRenderer::Init(const SModelInitData& modelInitData) noexcept
			{
				CreateRenderer(modelInitData);

				UpdateWorldMatrix();

				return;
			}

			void CModelRenderer::Terminate() noexcept
			{
				if (m_renderer)
				{
					m_renderer->DisableDrawing();
					delete m_renderer;
					m_renderer = nullptr;
				}
				return;
			}

			void CModelRenderer::CreateRenderer(const SModelInitData& modelInitData) noexcept
			{
				std::string msg = {};

				switch (modelInitData.rendererType)
				{
				case CRendererTable::EnRendererType::enBasicModel:
					m_renderer = new nsModels::CBasicModelRenderer(modelInitData);
					break;
				case CRendererTable::EnRendererType::enSkyCube:
					break;
				default:
					msg = modelInitData.modelFilePath;
					msg += "のモデルのロードに失敗しました。SModelInitDataのrendererTypeが間違っています。";
					msg.erase(msg.end() - 1);
					nsGameWindow::MessageBoxError(nsUtils::GetWideStringFromString(msg).c_str());
					break;
				}

				return;
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