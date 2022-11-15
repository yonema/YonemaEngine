#pragma once
#include "../Renderers/GenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			class CFBXGenericRendererFBX_SDK : public nsRenderers::IGenericRenderer
			{
			private:
				enum class EnDescRangeType : unsigned int
				{
					enCbvForModelData,
					enCbvForMaterialData,
					enSrvForMaterialData,
					enNumDescRangeTypes
				};
				enum class EnRootParameterType : unsigned int
				{
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
				constexpr CFBXGenericRendererFBX_SDK() = default;
				~CFBXGenericRendererFBX_SDK() = default;
				bool Init() override;

			private:
				bool CreateRootSignature(ID3D12Device5* device) override;
				bool CreatePipelineState(ID3D12Device5* device) override;

			private:

			};
		}
	}
}
