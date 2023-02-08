#pragma once
#include "../Models/BasicGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsEnvironment
		{
			class CSkyCubeGenericRenderer : public nsModels::CBasicGenericRenderer
			{
			private:
				enum class EnDescRangeType : unsigned int
				{
					enCbvForModel,
					enSrvForMaterial,
					enNumDescRangeTypes
				};
				enum class EnRootParameterType : unsigned int
				{
					enModel,
					enMaterial,
					enNumRootParamerterTypes
				};
				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			private:
				void CreateRootParameter(
					std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
					std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
				) const noexcept override;

				inline std::wstring CreateRootSignatureName() const noexcept override
				{
					return L"SkyCubeGenericRenderer";
				}

				inline void CreateSampler(
					std::vector<CD3DX12_STATIC_SAMPLER_DESC>* pSamplerDescs) const noexcept override
				{
					pSamplerDescs->resize(1);
					pSamplerDescs->at(0).Init(
						0,
						D3D12_FILTER_MIN_MAG_MIP_LINEAR,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						0,
						0,
						D3D12_COMPARISON_FUNC_NEVER,	// ƒŠƒTƒ“ƒvƒŠƒ“ƒO‚µ‚È‚¢
						D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
						0.0f,
						D3D12_FLOAT32_MAX,
						D3D12_SHADER_VISIBILITY_PIXEL
					);
				};

				bool CreateShader(
					nsDx12Wrappers::CBlob* pVsBlob,
					nsDx12Wrappers::CBlob* pPsBlob
				) const noexcept override;


			public:
				constexpr CSkyCubeGenericRenderer() = default;
				virtual ~CSkyCubeGenericRenderer() = default;

			};

		}
	}
}