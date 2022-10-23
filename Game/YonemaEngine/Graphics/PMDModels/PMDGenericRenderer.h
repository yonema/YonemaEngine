#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{
			class CPMDGenericRenderer : private nsUtils::SNoncopyable
			{
			private:
				static const unsigned int m_kNumDescTableRanges;
				static const unsigned int m_kNumRootParameters;
				static const unsigned int m_kNumSamplers;

			public:
				CPMDGenericRenderer();
				~CPMDGenericRenderer();

				constexpr ID3D12RootSignature* GetRootSignature() noexcept
				{
					return m_rootSignature;
				}

				constexpr ID3D12PipelineState* GetPipelineState() noexcept
				{
					return m_pipelineState;
				}

			private:
				void Init();
				void Terminate();
				void CreateRootSignature(ID3D12Device5* device);
				void CreatePipelineState(ID3D12Device5* device);
				bool LoadShader(ID3DBlob** pVsBlob, ID3DBlob** pPsBlob);

				bool CheckShaderCompileResult(HRESULT result, ID3DBlob* error = nullptr) noexcept;


			private:
				ID3D12RootSignature* m_rootSignature = nullptr;
				ID3D12PipelineState* m_pipelineState = nullptr;
			};

		}
	}
}