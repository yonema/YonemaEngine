#pragma once
#include "InstancingGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			class CInstancingNonCullingGenericRenderer : public CInstancingGenericRenderer
			{
			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"InstancingNonCullingGenericRenderer";
				}

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"InstancingNonCullingGenericRenderer";
				}

				inline D3D12_CULL_MODE SetUpCullMode() const noexcept override
				{
					return D3D12_CULL_MODE_NONE;
				};

			public:
				constexpr CInstancingNonCullingGenericRenderer() = default;
				~CInstancingNonCullingGenericRenderer() = default;

			private:

			};

		}
	}
}