#pragma once
#include "../Sprites/Sprite.h"
#include "../Camera.h"
#include "../ImageProcessing/GaussianBlur.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class CRendererTable;
			class CSpriteRender;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{

			class CShadowMapRenderer : private nsUtils::SNoncopyable
			{
			private:
				static const float m_kShadowMapSize;
				static const nsMath::CVector4 m_kClearColor;
				static const DXGI_FORMAT m_kDepthStencilFormat;
				static const D3D12_RESOURCE_STATES m_kResourceState;

				static const float m_kShadowCameraOrthographicProjectionSize;
				static const float m_kShadowCameraNearClip;
				static const float m_kShadowCameraFarClip;
				static const float m_kShadowCameraOffset;
				static const float m_kShadowCameraOffsetByAngle;
				static const float m_kShadowCameraHeight;

				static const float m_kBlurPower;

			public:
				inline CShadowMapRenderer() = default;
				~CShadowMapRenderer();

				void Init();

				void Release();

				void ExecuteDraw(
					nsDx12Wrappers::CCommandList* commandList,
					nsRenderers::CRendererTable* rendererTable
				);

				void Update(const nsMath::CVector3& lightDir);

				constexpr auto* GetCamera() noexcept
				{
					return &m_shadowCamera;
				}

				constexpr auto* GetShadowMapSprite() noexcept
				{
					return &m_shadowMapSprite;
				}

				constexpr auto* GetShadowBokeTexture() noexcept
				{
					return m_blur.GetBokeTexture();
				}

			private:

				void Terminate();

				void InitShadowCamera();

			private:
				nsDx12Wrappers::CRenderTarget m_shadowMapRT = {};
				nsSprites::CSprite m_shadowMapSprite = {};
				CCamera m_shadowCamera = {};
				nsImageProcessing::CGaussianBlur m_blur = {};
			};
		}
	}
}
