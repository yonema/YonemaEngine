#include "GenericRenderer.h"
#include "GenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			IGenericRenderer::~IGenericRenderer()
			{
				Terminate();
				return;
			}

			void IGenericRenderer::Terminate()
			{
				Release();
				return;
			}


			void IGenericRenderer::Release()
			{
				m_pipelineState.Release();
				m_rootSignature.Release();
				return;
			}

			bool IGenericRenderer::Init()
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				if (CreateRootSignature(device) != true)
				{
					return false;
				}
				if (CreatePipelineState(device) != true)
				{
					return false;
				}

				return true;
			}

			bool IGenericRenderer::CreateRootSignature(ID3D12Device5* device)
			{
				std::vector<CD3DX12_DESCRIPTOR_RANGE1> descTblRanges = {};
				std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};
				CreateRootParameter(&descTblRanges, &rootParameters);

				std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplerDescs = {};
				CreateSampler(&samplerDescs);


				auto res = m_rootSignature.Init(
					static_cast<unsigned int>(rootParameters.size()),
					rootParameters.data(),
					static_cast<unsigned int>(samplerDescs.size()),
					samplerDescs.data(),
					CreateRootSignatureFlags(),
					CreateRootSignatureName().c_str()
				);

				return res;
			}

			bool IGenericRenderer::CreatePipelineState(ID3D12Device5* device)
			{
				nsDx12Wrappers::CBlob vsBlob;
				nsDx12Wrappers::CBlob psBlob;
				auto res = CreateShader(&vsBlob, &psBlob);
				if (res != true)
				{
					return false;
				}

				std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {};
				CreateInputLayout(&inputLayout);


				D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

				// ìnÇ∑ílÇ™Ç∑Ç≈Ç…â¬ïœ
				pipelineDesc.pRootSignature = m_rootSignature.Get();
				pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
				pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
				pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();
				pipelineDesc.InputLayout.NumElements = 
					static_cast<unsigned int>(inputLayout.size());

				// ïœçXÇµÇ»Ç≠ÇƒÇ‡Ç¢Ç¢
				pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
				pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
				pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

				// â¬ïœÇ…Ç∑ÇÈ
				pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				OverrideBlendState(&pipelineDesc.BlendState);

				pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				pipelineDesc.RasterizerState.CullMode = SetUpCullMode();
				pipelineDesc.PrimitiveTopologyType = SetUpPrimitiveTopologyType();

				CreateDepthStencilState(&pipelineDesc.DepthStencilState, &pipelineDesc.DSVFormat);
				CreateRenderTargetFomrmat(pipelineDesc.RTVFormats, &pipelineDesc.NumRenderTargets);
				CreateSampleDesc(&pipelineDesc.SampleDesc);

				res = m_pipelineState.InitAsGraphicsPipelineState(
					pipelineDesc,
					CreatePipelineStateName().c_str()
				);

				return res;
			}

			bool IGenericRenderer::LoadShader(
				const wchar_t* vsFilePath,
				const char* vsEntryFuncName,
				nsDx12Wrappers::CBlob* pVsBlob,
				const wchar_t* psFilePath,
				const char* psEntryFuncName,
				nsDx12Wrappers::CBlob* pPsBlob
			) const noexcept
			{
				nsDx12Wrappers::CShader shader;

				if (shader.LoadAsVertexShader(vsFilePath, vsEntryFuncName, pVsBlob) != true)
				{
					return false;
				}
				if (shader.LoadAsPixelShader(psFilePath, psEntryFuncName, pPsBlob) != true)
				{
					return false;
				}

				return true;
			}


		}
	}
}