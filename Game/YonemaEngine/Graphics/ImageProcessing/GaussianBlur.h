#pragma once
#include "../Sprites/Sprite.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			class CGaussianBlur : private nsUtils::SNoncopyable
			{
			private:
				static const D3D12_RESOURCE_STATES m_kXBlurResourceState;
				static const unsigned int m_kNumWeights = 8;

			public:
				constexpr CGaussianBlur() = default;
				~CGaussianBlur();

				void Release();

				void Init(nsDx12Wrappers::CTexture* originalTexture, float blurPower);

				void ExecuteOnGPU(nsDx12Wrappers::CCommandList* commandList);

				constexpr auto* GetBokeTexture() noexcept
				{
					return m_yBlurRenderTarget.GetRenderTargetTexture();
				}

			private:
				void Terminate();

				void InitRenderTargets();

				void InitSprites();

				void InitCBV();

				void UpdateWeightsTable(float blurPower);

			private:
				float m_weights[m_kNumWeights] = {};
				nsDx12Wrappers::CTexture* m_originalTexture = nullptr;
				nsDx12Wrappers::CRenderTarget m_xBlurRenderTarget;
				nsDx12Wrappers::CRenderTarget m_yBlurRenderTarget;
				nsSprites::CSprite m_xBlurSprite;
				nsSprites::CSprite m_yBlurSprite;
				nsDx12Wrappers::CConstantBuffer m_XBlurCB = {};
				nsDx12Wrappers::CConstantBuffer m_YBlurCB = {};
			};



		}
	}
}