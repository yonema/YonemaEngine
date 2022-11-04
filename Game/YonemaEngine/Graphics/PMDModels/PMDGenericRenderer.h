#pragma once
#include "../Renderers/GenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{
			class CPMDGenericRenderer : public nsRenderers::IGenericRenderer
			{
			private:
				enum class EnDescRangeType : unsigned int
				{
					enCbvForSceneData,
					enCbvForModelData,
					enCbvForMaterialData,
					enSrvForMaterialData,
					enNumDescRangeTypes
				};
				enum class EnRootParameterType : unsigned int
				{
					enSceneData,
					enModelData,
					enMaterialData,
					enNumRootParamerterTypes
				};
				enum class EnSamplerType : unsigned int
				{
					enNormal,
					enNumSamplerTypes
				};

				static const wchar_t* const m_kVsFilePath;
				static const char* const m_kVsEntryFuncName;
				static const wchar_t* const m_kPsFilePath;
				static const char* const m_kPsEntryFuncName;

			public:
				constexpr CPMDGenericRenderer() = default;
				~CPMDGenericRenderer() = default;
				bool Init() override;

			private:
				bool CreateRootSignature(ID3D12Device5* device) override;
				bool CreatePipelineState(ID3D12Device5* device) override;

			private:

			};

		}
	}
}