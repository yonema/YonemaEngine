#pragma once
#include "GaussianBlurBaseGenericRenderer.h"
#include "../Shadow/ShadowMapGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			class CGaussianBlurYGenericRenderer : public CGaussianBlurBaseGenericRenderer
			{
			private:
				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			private:
				bool CreateShader(
					nsDx12Wrappers::CBlob* pVsBlob,
					nsDx12Wrappers::CBlob* pPsBlob
				) const noexcept override;

				inline void CreateRenderTargetFomrmat(
					DXGI_FORMAT rtvFormats[], UINT* pNumRenderTargets) const noexcept override
				{
					*pNumRenderTargets = 1;
					rtvFormats[0] = nsShadow::CShadowMapGenericRenderer::m_kRTVFormat;
				}

			public:
				constexpr CGaussianBlurYGenericRenderer() = default;
				virtual ~CGaussianBlurYGenericRenderer() = default;
			};
		}
	}
}
