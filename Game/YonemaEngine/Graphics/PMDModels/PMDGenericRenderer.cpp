#include "PMDGenericRenderer.h"
#include "../GameWindow/MessageBox.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsPMDModels
		{
			const unsigned int CPMDGenericRenderer::m_kNumDescTableRanges = 4;
			const unsigned int CPMDGenericRenderer::m_kNumRootParameters = 3;
			const unsigned int CPMDGenericRenderer::m_kNumSamplers = 1;

			CPMDGenericRenderer::CPMDGenericRenderer()
			{
				Init();

				return;
			}
			CPMDGenericRenderer::~CPMDGenericRenderer()
			{
				Terminate();

				return;
			}

			void CPMDGenericRenderer::Init()
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				CreateRootSignature(device);
				CreatePipelineState(device);

				return;
			}

			void CPMDGenericRenderer::Terminate()
			{
				if (m_pipelineState)
				{
					m_pipelineState->Release();
				}
				if (m_rootSignature)
				{
					m_rootSignature->Release();
				}

				return;
			}

			void CPMDGenericRenderer::CreateRootSignature(ID3D12Device5* device)
			{
				CD3DX12_DESCRIPTOR_RANGE1 descTblRanges[m_kNumDescTableRanges] = {};
				// �萔[b0]�i�r���[�v���W�F�N�V�����p�j
				descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
				// �萔[b1]�i���[���h�A�{�[���p�j
				descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
				// �萔[b2]�i�}�e���A���p�j
				// �f�B�X�N���v�^�q�[�v�͕������邪�A��x�Ɏg���͈̂�B
				descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
				//�e�N�X�`��[t0]�i�e�N�X�`��3�i��{��sph��spa�j�j
				descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);


				CD3DX12_ROOT_PARAMETER1 rootParameters[m_kNumRootParameters] = {};
				// �r���[�v���W�F�N�V�����ϊ�
				rootParameters[0].InitAsDescriptorTable(
					1, &descTblRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);
				// ���[���h�E�{�[���ϊ�
				rootParameters[1].InitAsDescriptorTable(
					1, &descTblRanges[1], D3D12_SHADER_VISIBILITY_VERTEX);
					//1, &descTblRanges[1], D3D12_SHADER_VISIBILITY_ALL);
				// �}�e���A������
				rootParameters[2].InitAsDescriptorTable(
					2, &descTblRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);


				CD3DX12_STATIC_SAMPLER_DESC samplerDescs[m_kNumSamplers] = {};
				samplerDescs[0].Init(
					0,
					D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					0,
					16,
					D3D12_COMPARISON_FUNC_NEVER,	// ���T���v�����O���Ȃ�
					D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
					0.0f,
					D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY_PIXEL,
					0
				);
				

				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
				rootSignatureDesc.Init_1_1(
					m_kNumRootParameters,
					rootParameters,
					m_kNumSamplers,
					samplerDescs,
					// ���_���i���̓A�Z���u���j������B
					D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | 
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | 
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
				);

				ID3DBlob* rootSignatureBlob = nullptr;
				ID3DBlob* errorBlob = nullptr;

				auto result = D3DX12SerializeVersionedRootSignature(
					&rootSignatureDesc,
					D3D_ROOT_SIGNATURE_VERSION_1_1,
					&rootSignatureBlob,
					&errorBlob
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"���[�g�V�O�l�`���̃o�C�i���R�[�h�̐����Ɏ��s���܂����B");
				}

				result = device->CreateRootSignature(
					0,
					rootSignatureBlob->GetBufferPointer(),
					rootSignatureBlob->GetBufferSize(),
					IID_PPV_ARGS(&m_rootSignature)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"���[�g�V�O�l�`���̐����Ɏ��s���܂����B");
				}

				CheckShaderCompileResult(result, errorBlob);

				// �s�v�ɂȂ������߉���B
				rootSignatureBlob->Release();

				return;
			}

			void CPMDGenericRenderer::CreatePipelineState(ID3D12Device5* device)
			{
				ID3DBlob* vsBlob = nullptr;
				ID3DBlob* psBlob = nullptr;

				LoadShader(&vsBlob, &psBlob);

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
						"NORMAL",
						0,
						DXGI_FORMAT_R32G32B32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"TEXCOORD",	// UV
						0,
						DXGI_FORMAT_R32G32_FLOAT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"BONE_NO",
						0,
						DXGI_FORMAT_R16G16_UINT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"WEIGHT",
						0,
						DXGI_FORMAT_R8_UINT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					},
					{
						"EDGE_FLG",
						0,
						DXGI_FORMAT_R8_UINT,
						0,
						D3D12_APPEND_ALIGNED_ELEMENT,
						D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
						0
					}
				};

				D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
				pipelineDesc.pRootSignature = m_rootSignature;
				pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);
				pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);
				pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

				pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

				pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

				pipelineDesc.DepthStencilState.DepthEnable = true;
				pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
				pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
				pipelineDesc.DepthStencilState.StencilEnable = false;

				pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
				pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

				pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
				pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

				pipelineDesc.NumRenderTargets = 1;
				pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

				// AA���Ȃ��ݒ�
				pipelineDesc.SampleDesc.Count = 1;
				pipelineDesc.SampleDesc.Quality = 0;

				pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

				auto result = device->CreateGraphicsPipelineState(
					&pipelineDesc,
					IID_PPV_ARGS(&m_pipelineState)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"�O���t�B�b�N�X�p�C�v���C���X�e�[�g�̐����Ɏ��s���܂����B");
				}


				return;
			}

			bool CPMDGenericRenderer::LoadShader(ID3DBlob** pVsBlob, ID3DBlob** pPsBlob)
			{
				ID3DBlob* errorBlob = nullptr;

				auto result = D3DCompileFromFile(
					L"Assets/Shaders/BasicVertexShader.hlsl",
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�\�ɂ��Ă���
					"BasicVS",
					"vs_5_0",
					// @todo �V�F�[�_�[�̃t���O���f�o�b�O�p����эœK���Ȃ��ɂȂ��Ă邩�炠�ƂŒ����B
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�N�p����эœK���Ȃ�
					0,
					pVsBlob,
					&errorBlob
				);
				if (CheckShaderCompileResult(result, errorBlob) != true)
				{
					return false;
				}
				result = D3DCompileFromFile(
					L"Assets/Shaders/BasicPixelShader.hlsl",
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�\�ɂ��Ă���
					"BasicPS",
					"ps_5_0",
					D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	// �f�o�b�N�p����эœK���Ȃ�
					0,
					pPsBlob,
					&errorBlob
				);
				if (CheckShaderCompileResult(result, errorBlob) != true)
				{
					return false;
				}

				return true;
			}



			bool CPMDGenericRenderer::CheckShaderCompileResult(HRESULT result, ID3DBlob* error) 
			{
				if (FAILED(result)) 
				{
					if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) 
					{
						::OutputDebugStringA("�t�@�C������������܂���");
					}
					else 
					{
						std::string errstr;
						errstr.resize(error->GetBufferSize());
						std::copy_n((char*)error->GetBufferPointer(), error->GetBufferSize(), errstr.begin());
						errstr += "\n";
						::OutputDebugStringA(errstr.c_str());
					}

					if (error)
					{
						error->Release();
					}
					return false;
				}
				else 
				{
					if (error)
					{
						error->Release();
					}
					return true;
				}
			}



		}
	}
}