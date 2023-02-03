#pragma once
#include "ShadowMapGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			class CShadowMapInstancingGenericRenderer : public CShadowMapGenericRenderer
			{
			private:
				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"ShadowMapInstancingGenericRenderer";
				}

				bool CreateShader(
					nsDx12Wrappers::CBlob* pVsBlob,
					nsDx12Wrappers::CBlob* pPsBlob
				) const noexcept override;

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"ShadowMapInstancingGenericRenderer";
				}

			public:
				constexpr CShadowMapInstancingGenericRenderer() = default;
				~CShadowMapInstancingGenericRenderer() = default;

			private:

			};

		}
	}
}