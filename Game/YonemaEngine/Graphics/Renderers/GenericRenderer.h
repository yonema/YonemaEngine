#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class IGenericRenderer : private nsUtils::SNoncopyable
			{
			private:
				virtual bool CreateRootSignature(ID3D12Device5* device) = 0;
				virtual bool CreatePipelineState(ID3D12Device5* device) = 0;

			public:
				constexpr IGenericRenderer() = default;
				virtual ~IGenericRenderer();
				virtual bool Init() = 0;

				void Release();

				constexpr auto GetRootSignature() noexcept
				{
					return &m_rootSignature;
				}

				constexpr auto GetPipelineState() noexcept
				{
					return &m_pipelineState;
				}

			protected:
				bool LoadShader(
					const wchar_t* vsFilePath,
					const char* vsEntryFuncName,
					nsDx12Wrappers::CBlob* pVsBlob,
					const wchar_t* psFilePath,
					const char* psEntryFuncName,
					nsDx12Wrappers::CBlob* pPsBlob
				);



			private:
				void Terminate();

			private:
				nsDx12Wrappers::CRootSignature m_rootSignature;
				nsDx12Wrappers::CPipelineState m_pipelineState;
			};
		}
	}
}