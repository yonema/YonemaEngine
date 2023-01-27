#pragma once
#include "BasicGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			class CBasicNonCullingGenericRenderer : public CBasicGenericRenderer
			{
			private:
				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"BasicNonCullingGenericRenderer";
				}

				inline std::wstring CreatePipelineStateName() const noexcept override
				{
					return L"BasicNonCullingGenericRenderer";
				}

				inline D3D12_CULL_MODE SetUpCullMode() const noexcept override
				{
					return D3D12_CULL_MODE_NONE;
				};

			public:
				constexpr CBasicNonCullingGenericRenderer() = default;
				~CBasicNonCullingGenericRenderer() = default;

			private:

			};

		}
	}
}