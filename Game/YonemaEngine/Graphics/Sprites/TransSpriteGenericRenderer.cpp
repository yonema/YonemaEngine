#include "TransSpriteGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsSprites
		{
			void CTransSpriteGenericRenderer::OverrideGraphicsPipelineStateDesc(
				D3D12_GRAPHICS_PIPELINE_STATE_DESC* pPipelineDesc)
			{
				// ”¼“§–¾‡¬—p‚ÌÝ’è‚Åã‘‚«
				pPipelineDesc->BlendState.RenderTarget[0].BlendEnable = true;
				pPipelineDesc->BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				pPipelineDesc->BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				pPipelineDesc->BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

				return;
			}
		}
	}
}