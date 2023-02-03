#pragma once
#include "GaussianBlurBaseGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			class CGaussianBlurXGenericRenderer : public CGaussianBlurBaseGenericRenderer
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

			public:
				constexpr CGaussianBlurXGenericRenderer() = default;
				virtual ~CGaussianBlurXGenericRenderer() = default;
			};
		}
	}
}
