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

				virtual void CreateRootParameter(
					std::vector<CD3DX12_DESCRIPTOR_RANGE1>* pDescTblRanges,
					std::vector<CD3DX12_ROOT_PARAMETER1>* pRootParameters
				) const noexcept {};

				virtual inline void CreateSampler(
					std::vector<CD3DX12_STATIC_SAMPLER_DESC>* pSamplerDescs) const noexcept
				{
					pSamplerDescs->resize(1);
					pSamplerDescs->at(0).Init(
						0,
						D3D12_FILTER_ANISOTROPIC,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						D3D12_TEXTURE_ADDRESS_MODE_WRAP,
						0,
						16,
						D3D12_COMPARISON_FUNC_NEVER,	// リサンプリングしない
						D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
						0.0f,
						D3D12_FLOAT32_MAX,
						D3D12_SHADER_VISIBILITY_PIXEL
					);
				}
				virtual inline D3D12_ROOT_SIGNATURE_FLAGS CreateRootSignatureFlags() const noexcept
				{
					// 頂点情報（入力アセンブラ）がある。
					return D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
						D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
						D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
						D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
				}
				virtual inline std::wstring CreateRootSignatureName() const noexcept 
				{
					return L"GenericRenderer";
				}

				virtual bool CreateShader(
					nsDx12Wrappers::CBlob* pVsBlob, nsDx12Wrappers::CBlob* pPsBlob) const noexcept
				{
					return false;
				};

				virtual void CreateInputLayout(
					std::vector<D3D12_INPUT_ELEMENT_DESC>* pInputLayout) const noexcept {};

				virtual inline void OverrideBlendState(
					D3D12_BLEND_DESC* pBlendState) const noexcept {};

				virtual inline D3D12_CULL_MODE SetUpCullMode() const noexcept 
				{
					return D3D12_CULL_MODE_BACK;
				};

				virtual inline D3D12_PRIMITIVE_TOPOLOGY_TYPE SetUpPrimitiveTopologyType() const noexcept
				{
					return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				}

				virtual inline void CreateDepthStencilState(
					D3D12_DEPTH_STENCIL_DESC* pDepthStencilState,
					DXGI_FORMAT* pDSVFormat
				) const noexcept
				{
					pDepthStencilState->DepthEnable = true;
					pDepthStencilState->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
					pDepthStencilState->DepthFunc = D3D12_COMPARISON_FUNC_LESS;
					pDepthStencilState->StencilEnable = false;
					*pDSVFormat = DXGI_FORMAT_D32_FLOAT;
					return;
				}

				virtual inline void CreateRenderTargetFomrmat(
					DXGI_FORMAT rtvFormats[], UINT* pNumRenderTargets) const noexcept
				{
					*pNumRenderTargets = 1;
					rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
				}

				virtual inline void CreateSampleDesc(DXGI_SAMPLE_DESC* pSampleDesc) const noexcept
				{
					// AAしない設定
					pSampleDesc->Count = 1;
					pSampleDesc->Quality = 0;
					return;
				}

				virtual inline std::wstring CreatePipelineStateName() const noexcept
				{
					return L"GenericRenderer";
				}

			public:
				constexpr IGenericRenderer() = default;
				virtual ~IGenericRenderer();

				bool Init();

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
				) const noexcept;



			private:
				void Terminate();
				bool CreateRootSignature(ID3D12Device5* device);
				bool CreatePipelineState(ID3D12Device5* device);

			private:
				nsDx12Wrappers::CRootSignature m_rootSignature;
				nsDx12Wrappers::CPipelineState m_pipelineState;
			};
		}
	}
}