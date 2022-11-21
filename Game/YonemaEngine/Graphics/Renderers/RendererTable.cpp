#include "RendererTable.h"
#include "../PMDModels/PMDGenericRenderer.h"
#include "../FBXModels/FBXGenericRendererFBX_SDK.h"
#include "../FBXModels/FBXGenericRendererAssimp.h"
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
				m_genericRendererTable[static_cast<int>(EnRendererType::enPMDModel)] =
					new nsPMDModels::CPMDGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enFBXModel)] =
					//new nsFBXModels::CFBXGenericRendererFBX_SDK();
					new nsFBXModels::CFBXGenericRendererAssimp();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSimplePostEffect)] =
					new CSimplePostEffectGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSprite)] =
					new nsSprites::CSpriteGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enTransSprite)] =
					new nsSprites::CTransSpriteGenericRenderer();

				for (auto genericRenderer : m_genericRendererTable)
				{
					genericRenderer->Init();
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