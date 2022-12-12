#pragma once
#include "SpriteGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsSprites
		{
			class CTransSpriteGenericRenderer : public CSpriteGenericRenderer
			{
			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"TransSpriteGenericRenderer";
				}
				void OverrideBlendState(D3D12_BLEND_DESC* pBlendState) const noexcept override
				{
					// ”¼“§–¾‡¬—p‚ÌÝ’è‚Åã‘‚«
					pBlendState->RenderTarget[0].BlendEnable = true;
					pBlendState->RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
					pBlendState->RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
					pBlendState->RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				}

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"TransSpriteGenericRenderer";
				}

			public:
				constexpr CTransSpriteGenericRenderer() = default;
				~CTransSpriteGenericRenderer() = default;

			private:

			};
		}
	}
}
