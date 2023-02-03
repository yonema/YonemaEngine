#include "RendererTable.h"
#include "../Shadow/ShadowMapGenericRenderer.h"
#include "../Shadow/ShadowMapSkinGenericRenderer.h"
#include "../Shadow/ShadowMapInstancingGenericRenderer.h"
#include "../Models/BasicGenericRenderer.h"
#include "../Models/SkinGenericRenderer.h"
#include "../Models/InstancingGenericRenderer.h"
#include "../Models/BasicNonCullingGenericRenderer.h"
#include "../Models/SkinNonCullingGenericRenderer.h"
#include "../Models/InstancingNonCullingGenericRenderer.h"
#include "../DebugRenderers/PhysicsDebugLineGenericRenderer.h"
#include "../Sprites/SpriteGenericRenderer.h"
#include "../Sprites/TransSpriteGenericRenderer.h"
#include "../SimplePostEffectGenericRenderer.h"
#include "../ImageProcessing/GaussianBlurXGenericRenderer.h"
#include "../ImageProcessing/GaussianBlurYGenericRenderer.h"

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
				m_genericRendererTable[static_cast<int>(EnRendererType::enShadowModel)] =
					new nsShadow::CShadowMapGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enShadowSkinModel)] =
					new nsShadow::CShadowMapSkinGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enShadowInstancingModel)] =
					new nsShadow::CShadowMapInstancingGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enGaussianBlurXForShadowMap)] =
					new nsImageProcessing::CGaussianBlurXGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enGaussianBlurYForShadowMap)] =
					new nsImageProcessing::CGaussianBlurYGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enBasicModel)] =
					new nsModels::CBasicGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSkinModel)] =
					new nsModels::CSkinGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enInstancingModel)] =
					new nsModels::CInstancingGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enBasicNonCullingModel)] =
					new nsModels::CBasicNonCullingGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enSkinNonCullingModel)] =
					new nsModels::CSkinNonCullingGenericRenderer();
				m_genericRendererTable[static_cast<int>(EnRendererType::enInstancingNonCullingModel)] =
					new nsModels::CInstancingNonCullingGenericRenderer();
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