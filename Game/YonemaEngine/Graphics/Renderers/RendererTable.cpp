#include "RendererTable.h"
#include "../Models/BasicGenericRenderer.h"
#include "../Models/SkinGenericRenderer.h"
#include "../DebugRenderers/PhysicsDebugLineGenericRenderer.h"
#include "../Sprites/SpriteGenericRenderer.h"
#include "../Sprites/TransSpriteGenericRenderer.h"
#include "../SimplePostEffectGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			CRendererTable::~CRendererTable()
			{
				Terminate();
				return;
			}

			void CRendererTable::Init()
			{
				InitGenericRenderTable();

				return;
			}

			void CRendererTable::InitGenericRenderTable()
			{
				m_genericRendererTable[static_cast<int>(EnRendererType::enBasicModel)] =
					new nsModels::CBasicGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSkinModel)] =
					new nsModels::CSkinGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enCollisionRenderer)] =
					new nsDebugRenderers::CPhysicsDebugLineGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSimplePostEffect)] =
					new CSimplePostEffectGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSprite)] =
					new nsSprites::CSpriteGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enTransSprite)] =
					new nsSprites::CTransSpriteGenericRenderer();

				for (auto genericRenderer : m_genericRendererTable)
				{
					if (genericRenderer)
					{
						genericRenderer->Init();
					}
				}

				return;
			}


			void CRendererTable::Terminate()
			{
				for (auto genericRender : m_genericRendererTable)
				{
					if (genericRender)
					{
						delete genericRender;
					}
				}
				return;
			}


		}
	}
}