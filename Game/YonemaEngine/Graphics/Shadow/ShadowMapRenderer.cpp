#include "ShadowMapRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			const unsigned int CShadowMapRenderer::m_kShadowMapHeight = 512;
			const unsigned int CShadowMapRenderer::m_kShadowMapWidth = 512;
			const DXGI_FORMAT CShadowMapRenderer::m_kShadowMapFormat = DXGI_FORMAT_R32G32_FLOAT;
			const nsMath::CVector4 CShadowMapRenderer::m_kClearColor = { -1.0f,-1.0f,-1.0f,1.0f };
			const DXGI_FORMAT CShadowMapRenderer::m_kDepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

			void CShadowMapRenderer::Init()
			{
				auto res = m_shadowMapRT.Init(
					m_kShadowMapHeight,
					m_kShadowMapWidth,
					m_kShadowMapFormat,
					m_kClearColor,
					m_kDepthStencilFormat,
					L"ShadowMapRenderTarget"
				);


				nsSprites::SSpriteInitData initData;
				initData.texture = m_shadowMapRT.GetRenderTargetTexture();
				initData.spriteSize = initData.texture->GetTextureSize();
				res = m_shadowMapSprite.Init(initData);


				return;
			}

			CShadowMapRenderer::~CShadowMapRenderer()
			{
				Terminate();

				return;
			}

			void CShadowMapRenderer::Terminate()
			{
				Release();

				return;
			}


			void CShadowMapRenderer::Release()
			{
				m_shadowMapSprite.Release();
				m_shadowMapRT.Release();

				return;
			}


			void CShadowMapRenderer::Draw()
			{


				return;
			}





		}
	}
}