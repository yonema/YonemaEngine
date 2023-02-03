#pragma once
#include "BasicGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			class CInstancingGenericRenderer : public CBasicGenericRenderer
			{
			private:
				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"InstancingGenericRenderer";
				}

				bool CreateShader(
					nsDx12Wrappers::CBlob* pVsBlob,
					nsDx12Wrappers::CBlob* pPsBlob
				) const noexcept override;

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"InstancingGenericRenderer";
				}

			public:
				constexpr CInstancingGenericRenderer() = default;
				~CInstancingGenericRenderer() = default;

			private:

			};

		}
	}
}