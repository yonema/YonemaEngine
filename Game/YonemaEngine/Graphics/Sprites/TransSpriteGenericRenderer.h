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
				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			public:
				constexpr CTransSpriteGenericRenderer() = default;
				~CTransSpriteGenericRenderer() = default;

			private:
				void OverrideGraphicsPipelineStateDesc(
					D3D12_GRAPHICS_PIPELINE_STATE_DESC* pPipelineDesc)override final;

			private:

			};
		}
	}
}
