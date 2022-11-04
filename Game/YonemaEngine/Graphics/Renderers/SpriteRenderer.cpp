#include "SpriteRenderer.h"
#include "../2D/Sprite.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			bool CSpriteRenderer::Start()
			{
				return true;
			}

			void CSpriteRenderer::Update(float deltaTime)
			{
				if (m_sprite)
				{
					m_sprite->UpdateWorldMatrix(m_position, m_rotation, m_scale, m_pivot);
				}
				return;
			}

			void CSpriteRenderer::OnDestroy()
			{
				Terminate();
				return;
			}
			void CSpriteRenderer::Init(const ns2D::SSpriteInitData& spriteInitData)
			{
				m_sprite = new ns2D::CSprite();

				m_sprite->Init(spriteInitData);

				if (spriteInitData.alphaBlendMode == ns2D::EnAlphaBlendMode::enTrans)
				{
					m_sprite->SetRenderType(CRendererTable::EnRendererType::enTransSprite);
				}
				else
				{
					m_sprite->SetRenderType(CRendererTable::EnRendererType::enSprite);
				}

				m_sprite->EnableDrawing();

				return;
			}

			void CSpriteRenderer::Terminate()
			{
				m_sprite->DisableDrawing();
				if (m_sprite)
				{
					delete m_sprite;
					m_sprite = nullptr;
				}
				return;
			}

		}
	}
}