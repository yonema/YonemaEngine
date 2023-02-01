#pragma once
#include "../Sprites/Sprite.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{

			class CShadowMapRenderer : private nsUtils::SNoncopyable
			{
			private:
				static const unsigned int m_kShadowMapHeight;
				static const unsigned int m_kShadowMapWidth;
				static const DXGI_FORMAT m_kShadowMapFormat;
				static const nsMath::CVector4 m_kClearColor;
				static const DXGI_FORMAT m_kDepthStencilFormat;

			public:
				constexpr CShadowMapRenderer() = default;
				~CShadowMapRenderer();

				void Init();

				void Release();

				void Draw();

			private:

				void Terminate();

			private:
				nsDx12Wrappers::CRenderTarget m_shadowMapRT = {};
				nsSprites::CSprite m_shadowMapSprite = {};
			};
		}
	}
}
