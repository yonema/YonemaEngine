#include "Renderer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			void IRenderer::EnableDrawing()
			{
				if (IsEnableDrawing())
				{
					return;
				}
				if (GetRenderType() == CRendererTable::EnRendererType::enNone)
				{
					return;
				}

				CGraphicsEngine::GetInstance()->GetRendererTable()->RegisterRenderer(
					GetRenderType(), this);

				m_isEnableDrawing = true;
				return;
			}

			void IRenderer::DisableDrawing()
			{
				if (IsEnableDrawing() != true)
				{
					return;
				}
				if (GetRenderType() == CRendererTable::EnRendererType::enNone)
				{
					return;
				}

				CGraphicsEngine::GetInstance()->GetRendererTable()->RemoveRenderer(
					GetRenderType(), this);
				m_isEnableDrawing = false;
				return;
			}

		}
	}
}