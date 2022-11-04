#include "RendererTable.h"
#include "../PMDModels/PMDGenericRenderer.h"
#include "../FBXModels/FBXGenericRenderer.h"
#include "../2D/SpriteGenericRenderer.h"
#include "../2D/TransSpriteGenericRenderer.h"
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
					new nsFBXModels::CFBXGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSimplePostEffect)] =
					new CSimplePostEffectGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSprite)] =
					new ns2D::CSpriteGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enTransSprite)] =
					new ns2D::CTransSpriteGenericRenderer();

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