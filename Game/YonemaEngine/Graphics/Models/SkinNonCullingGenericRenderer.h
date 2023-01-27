#pragma once
#include "SkinGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			class CSkinNonCullingGenericRenderer : public CSkinGenericRenderer
			{
			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"SkinNonCullingGenericRenderer";
				}

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"SkinNonCullingGenericRenderer";
				}

				inline D3D12_CULL_MODE SetUpCullMode() const noexcept override
				{
					return D3D12_CULL_MODE_NONE;
				};

			public:
				constexpr CSkinNonCullingGenericRenderer() = default;
				~CSkinNonCullingGenericRenderer() = default;

			private:

			};

		}
	}
}