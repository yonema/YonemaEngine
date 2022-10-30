#include "SpriteGenericRenderer.h"
#include "../GameWindow/MessageBox.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace ns2D
		{
			const wchar_t* const CSpriteGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/SpriteVertexShader.hlsl";
			const char* const CSpriteGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CSpriteGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/SpritePixelShader.hlsl";
			const char* const CSpriteGenericRenderer::m_kPsEntryFuncName = "PSMain";

			bool CSpriteGenericRenderer::Init()
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
			bool CSpriteGenericRenderer::CreateRootSignature(ID3D12Device5* device)
			{
				int numCBVs = 0;
				int numSRVs = 0;

				constexpr unsigned int kNumDescTblRanges =
					static_cast<unsigned int>(EnDescRangeType::enNumDescRangeTypes);
				constexpr unsigned int kCbvForSpriteDataDescRangeNo =
					static_cast<unsigned int>(EnDescRangeType::enCbvForSpriteData);
				constexpr unsigned int kSrvForSceneTextureDescRangeNo =
					static_cast<unsigned int>(EnDescRangeType::enSrvForSceneTexture);

				CD3DX12_DESCRIPTOR_RANGE1 descTableRanges[kNumDescTblRanges] = {};
				descTableRanges[kCbvForSpriteDataDescRangeNo].Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, numCBVs++);
				descTableRanges[kSrvForSceneTextureDescRangeNo].Init(
					D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, numSRVs++);

				constexpr unsigned int kNumRootParameters =
					static_cast<unsigned int>(EnRootParameterType::enNumRootParamerterTypes);
				constexpr unsigned int kSpriteDataRootParamNo =
					static_cast<unsigned int>(EnRootParameterType::enSpriteData);
				constexpr unsigned int kSceneTextureRootParamNo =
					static_cast<unsigned int>(EnRootParameterType::enSceneTexture);

				CD3DX12_ROOT_PARAMETER1 rootParameters[kNumRootParameters] = {};
				rootParameters[kSpriteDataRootParamNo].InitAsDescriptorTable(
					1, &descTableRanges[kCbvForSpriteDataDescRangeNo], D3D12_SHADER_VISIBILITY_ALL);
				rootParameters[kSceneTextureRootParamNo].InitAsDescriptorTable(
					1, &descTableRanges[kSrvForSceneTextureDescRangeNo], D3D12_SHADER_VISIBILITY_PIXEL);

				constexpr unsigned int kNumSamplers =
					static_cast<unsigned int>(EnSamplerType::enNumSamplerTypes);
				constexpr unsigned int kNormalSamplerNo =
					static_cast<unsigned int>(EnSamplerType::enNormal);

				CD3DX12_STATIC_SAMPLER_DESC samplerDescs[kNumSamplers] = {};
				samplerDescs[kNormalSamplerNo].Init(
					0,
					D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					0,
					16,
					D3D12_COMPARISON_FUNC_LESS_EQUAL,
					D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
					0.0f,
					D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY_PIXEL
				);

				auto res = GetRootSignature()->Init(
					kNumRootParameters,
					rootParameters,
					kNumSamplers,
					samplerDescs,
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS,
					L"Sprite"
				);

				return res;
			}
			bool CSpriteGenericRenderer::CreatePipelineState(ID3D12Device5* device)
			{
				nsDx12Wrappers::CBlob vsBlob;
				nsDx12Wrappers::CBlob psBlob;

				auto vsFilePath = m_kVsFilePath;
				auto vsEntryFuncName = m_kVsEntryFuncName;
				auto psFilePath = m_kPsFilePath;
				auto psEntryFuncName = m_kPsEntryFuncName;

				// 派生クラスでシェーダーの上書き
				OverrideShader(&vsFilePath, &vsEntryFuncName, &psFilePath, &psEntryFuncName);

				auto res = LoadShader(
					vsFilePath,
					vsEntryFuncName,
					&vsBlob,
					psFilePath,
					psEntryFuncName,
					&psBlob
				);
				if (res != true)
				{
					return false;
				}

				constexpr D3D12_INPUT_ELEMENT_DESC inputLayout[] =
				{
					{
						"POSITION",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"TEXCOORD",
						0,
						DXGI_FORMAT_R32G32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					}
				};

				D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
				pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
				pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

				pipelineDesc.pRootSignature = GetRootSignature()->Get();

				pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
				pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

				pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

				pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

				pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				pipelineDesc.DepthStencilState.DepthEnable = FALSE;
				pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				pipelineDesc.DepthStencilState.StencilEnable = FALSE;

				pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

				// AAしない設定
				pipelineDesc.SampleDesc.Count = 1;
				pipelineDesc.SampleDesc.Quality = 0;

				pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
				pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				pipelineDesc.NumRenderTargets = 1;

				pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

				// 派生クラスでパイプラインの設定の上書き。
				OverrideGraphicsPipelineStateDesc(&pipelineDesc);

				res = GetPipelineState()->InitAsGraphicsPipelineState(pipelineDesc, L"Sprite");

				return res;
			}
		}
	}
}