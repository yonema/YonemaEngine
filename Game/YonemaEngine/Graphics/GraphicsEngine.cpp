#include "GraphicsEngine.h"
#include "../GameWindow/MessageBox.h"
#include "../GameWindow/GameWindow.h"
#include "../../Application.h"
#include "Texture.h"
#include "PMDModels/PMDGenericRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		CGraphicsEngine* CGraphicsEngine::m_instance = nullptr;

		const wchar_t* CGraphicsEngine::m_kGPUAdapterNames[EnGPUAdapterType::NumGPUAdapterType] =
		{
			// �ォ��g�p�D��x��
			L"NVIDIA",
			L"AMD",
			L"Intel"
		};

		const D3D_FEATURE_LEVEL CGraphicsEngine::m_kFeatureLevels[m_kNumFeatureLevel]
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

		const DXGI_FORMAT CGraphicsEngine::m_kDepthFormat = DXGI_FORMAT_D32_FLOAT;
		const float CGraphicsEngine::m_kRTVClearColor[4] = { 0.5f,0.5f,0.5f,1.0f };

		CGraphicsEngine::~CGraphicsEngine()
		{
			Terminate();

			return;
		}


		bool CGraphicsEngine::Init()
		{
			IDXGIFactory6* dxgiFactory = nullptr;
			if (CreateDXGIFactory(&dxgiFactory) != true)
			{
				return false;
			}
			
			if (CreateDevice(dxgiFactory) != true)
			{
				return false;
			}

			if (CreateCommandAllocator() != true)
			{
				return false;
			}
			{
				// �R�}���h���X�g�ƃR�}���h�L���[�œ����^�C�v���g���B
				constexpr D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
				if (CreateCommandList(commandListType) != true)
				{
					return false;
				}

				if (CreateCommandQueue(commandListType) != true)
				{
					return false;
				}
			}

			if (CreateSwapChain(dxgiFactory) != true)
			{
				return false;
			}

			if (CreateDescriptorHeapForRTV() != true)
			{
				return false;
			}

			if (CreateRTVForFrameBuffer() != true)
			{
				return false;
			}

			if (CreateDescriptorHeapForDSV() != true)
			{
				return false;
			}

			if (CreateDSVForFrameBuffer() != true)
			{
				return false;
			}

			if (CreateFence() != true)
			{
				return false;
			}

			// ���������I�������ADXGIFactory�͂�������Ȃ����ߔj������B
			dxgiFactory->Release();

			const auto kWindowWidth = CApplication::GetInstance()->GetWindowWidth();
			const auto kWindowHeight = CApplication::GetInstance()->GetWindowHeight();

			m_viewport = CD3DX12_VIEWPORT(m_renderTargets[0]);
			m_scissorRect = CD3DX12_RECT(
				0,
				0,
				0 + kWindowWidth,
				0 + kWindowHeight
			);

			m_whiteTexture = new CTexture();
			m_blackTexture = new CTexture();
			m_whiteTexture->Init("Assets/Models/white.jpg");
			m_blackTexture->Init("Assets/Models/black.jpg");

			m_cameraPos = {0.0f, 10.0f, -15.0f};
			m_targetPos = { 0.0f, 10.0f, 0.0f };
			m_upDir = { 0.0f, 1.0f, 0.0f };
			m_mView.MakeViewMatrix(m_cameraPos, m_targetPos, m_upDir);
			m_mProj.MakeProjectionMatrix(
				nsMath::YM_PIDIV2,
				static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
				1.0f,
				100.0f
			);


			CreateSeceneConstantBuff();

			m_pmdGenericRenderer = new nsPMDModels::CPMDGenericRenderer();

			return true;
		}

		void CGraphicsEngine::Terminate()
		{
			if (m_pmdGenericRenderer)
			{
				delete m_pmdGenericRenderer;
			}
			if (m_whiteTexture)
			{
				delete m_whiteTexture;
			}
			if (m_blackTexture)
			{
				delete m_blackTexture;
			}
			if (m_sceneDataDescriptorHeap)
			{
				m_sceneDataDescriptorHeap->Release();
			}
			if (m_sceneDataConstantBuff)
			{
				m_sceneDataConstantBuff->Release();
			}
			if (m_fence)
			{
				m_fence->Release();
			}
			if (m_depthStencilBuffer)
			{
				m_depthStencilBuffer->Release();
			}
			if (m_dsvHeap)
			{
				m_dsvHeap->Release();
			}
			for (auto& renderTarget : m_renderTargets)
			{
				if (renderTarget)
				{
					renderTarget->Release();
				}
			}
			if (m_rtvHeap)
			{
				m_rtvHeap->Release();
			}
			if (m_swapChain)
			{
				m_swapChain->Release();
			}
			if (m_commandQueue)
			{
				m_commandQueue->Release();
			}
			if (m_commandList)
			{
				m_commandList->Release();
			}
			if (m_commandAllocator)
			{
				m_commandAllocator->Release();
			}
			if (m_device)
			{
				m_device->Release();
			}

			return;
		}

		void CGraphicsEngine::Update()
		{
			m_mView.MakeViewMatrix(m_cameraPos, m_targetPos, m_upDir);
			m_mappedSceneDataMatrix->mView = m_mView;
			m_mappedSceneDataMatrix->mProj = m_mProj;
			m_mappedSceneDataMatrix->cameraPosWS = m_cameraPos;

			return;
		}

		void CGraphicsEngine::BeginDraw()
		{
			// �����Z�b�g
			auto result = m_commandAllocator->Reset();
			result = m_commandList->Reset(m_commandAllocator, nullptr);

			auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();

			// �����\�[�X�o���A
			D3D12_RESOURCE_BARRIER barrierDesc =
				CD3DX12_RESOURCE_BARRIER::Transition(
					m_renderTargets[bbIdx],
					D3D12_RESOURCE_STATE_PRESENT,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);
			m_commandList->ResourceBarrier(1, &barrierDesc);

			// ��RTV��DSV�̃Z�b�g�ƃN���A
			auto rtvH = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
			rtvH.ptr += static_cast<long long unsigned int>(bbIdx) *
				m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto dsvH = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

			m_commandList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

			m_commandList->ClearRenderTargetView(rtvH, m_kRTVClearColor, 0, nullptr);
			m_commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// ���r���[�|�[�g�ƃV�U�[��`�̃Z�b�g
			m_commandList->RSSetViewports(1, &m_viewport);
			m_commandList->RSSetScissorRects(1, &m_scissorRect);

			// ���`�悷�郂�f���ɍ��킹�����[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g���Z�b�g
			m_commandList->SetGraphicsRootSignature(m_pmdGenericRenderer->GetRootSignature());
			m_commandList->SetPipelineState(m_pmdGenericRenderer->GetPipelineState());

			// ���v���~�e�B�u�g�|���W���Z�b�g
			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// ���V�[���f�[�^���Z�b�g
			m_commandList->SetDescriptorHeaps(1, &m_sceneDataDescriptorHeap);
			auto heapHandle = m_sceneDataDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			m_commandList->SetGraphicsRootDescriptorTable(0, heapHandle);

			return;
		}

		void CGraphicsEngine::EndDraw()
		{
			auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();

			D3D12_RESOURCE_BARRIER barrierDesc =
				CD3DX12_RESOURCE_BARRIER::Transition(
					m_renderTargets[bbIdx],
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PRESENT
				);
			m_commandList->ResourceBarrier(1, &barrierDesc);

			// ���߂̎��s�O�ɁA�K���R�}���h���X�g�̃N���[�Y���s���B
			m_commandList->Close();

			ID3D12CommandList* commandLists[] = { m_commandList };
			m_commandQueue->ExecuteCommandLists(1, commandLists);

			m_commandQueue->Signal(m_fence, ++m_fenceVal);
			if (m_fence->GetCompletedValue() != m_fenceVal)
			{
				auto eventH = CreateEvent(nullptr, false, false, nullptr);
				m_fence->SetEventOnCompletion(m_fenceVal, eventH);
				WaitForSingleObject(eventH, INFINITE);
				CloseHandle(eventH);
			}

			m_swapChain->Present(1, 0);

			return;
		}

		bool CGraphicsEngine::CreateDXGIFactory(IDXGIFactory6** dxgiFactory)
		{
			HRESULT result;
			UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
			EnableDebugLayer();	
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
			result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"DXGIFactory�I�u�W�F�N�g�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::EnableDebugLayer()
		{
			ID3D12Debug* debugLayer = nullptr;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

			// �f�o�b�N���C���[��L���ɂ���B
			debugLayer->EnableDebugLayer();
			// �L���ɂ�����A�C���^�[�t�F�[�X���J������B
			debugLayer->Release();
			return;
		}

		bool CGraphicsEngine::CreateDevice(IDXGIFactory6* dxgiFactory)
		{
			IDXGIAdapter* adapter = nullptr;
			FindAdapter(dxgiFactory, &adapter);

			for (auto featureLevel : m_kFeatureLevels)
			{
				auto hr = D3D12CreateDevice(adapter, featureLevel, IID_PPV_ARGS(&m_device));

				if (SUCCEEDED(hr))
				{
					// �����\�ȃo�[�W��������������A���[�v��ł��؂�B
					break;
				}
			}

			// �f�o�C�X�������I�������A�A�_�v�^�͂����g�p���Ȃ����߁A�j���B
			if (adapter)
			{
				adapter->Release();
			}

			if (m_device == nullptr)
			{
				nsGameWindow::MessageBoxError(L"�f�o�C�X�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::FindAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapterToUse)
		{
			// �Ō�ɂ܂Ƃ߂Ĕj�����邽�߂ɁA���ׂ��A�_�v�^���ׂĂ�ێ����Ă������X�g�B
			std::list<IDXGIAdapter*> adapters;
			// ���ׂ�Ώۂ̃A�_�v�^�B
			IDXGIAdapter* tempAdapter = nullptr;
			// GPU�̎�ޕʂ̃A�_�v�^�B
			// ������ނ̃A�_�v�^���������ꍇ�́A�r�f�I���������傫������ێ��B
			IDXGIAdapter* adaptersByType[EnGPUAdapterType::NumGPUAdapterType] = { nullptr };
			// ���ׂẴA�_�v�^�̒��ŁA1�ԃr�f�I���������傫���A�_�v�^��ێ��B
			IDXGIAdapter* maxVideoMemoryAdapter = nullptr;
			SIZE_T videoMomorySize = 0;

			// 1�F���p�\�ȃA�_�v�^��T��

			// PC�ɂ���A�_�v�^�����ꂼ�꒲�ׂ�
			for (int i = 0; dxgiFactory->EnumAdapters(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; i++)
			{
				adapters.emplace_back(tempAdapter);
				DXGI_ADAPTER_DESC tempAdapterDesc = {};
				tempAdapter->GetDesc(&tempAdapterDesc);

				// �D��x�̍����A�_�v�^�����邩���ׂ�
				for (int adapterType = 0; adapterType < EnGPUAdapterType::NumGPUAdapterType; adapterType++)
				{
					std::wstring strDesc = tempAdapterDesc.Description;
					if (strDesc.find(m_kGPUAdapterNames[adapterType]) == std::string::npos)
					{
						// ��ނ̖��O���Ⴄ�A���̎�ނցB
						continue;
					}

					if (adaptersByType[adapterType])
					{
						// ������ނ̃A�_�v�^���������������B
						// �r�f�I���������傫���ق����g�p����B

						DXGI_ADAPTER_DESC adaptersDesc;
						adaptersByType[adapterType]->GetDesc(&adaptersDesc);
						if (tempAdapterDesc.DedicatedVideoMemory <= adaptersDesc.DedicatedVideoMemory)
						{
							// �r�f�I�����������Ȃ�����A�g�p���Ȃ��B
							break;
						}
					}

					adaptersByType[adapterType] = tempAdapter;
						
					break;
					
				}


				// �r�f�I����������ԑ傫���A�_�v�^��T��
				if (tempAdapterDesc.DedicatedVideoMemory > videoMomorySize)
				{
					maxVideoMemoryAdapter = tempAdapter;
					videoMomorySize = tempAdapterDesc.DedicatedVideoMemory;
				}
			}


			// 2�F�T���o�����A�_�v�^����A�ǂ̃A�_�v�^���g�p���邩�I��

			bool decided = false;
			// �D��x�̍����A�_�v�^���珇�ɒ��ׂ�
			for (int i = 0; i < EnGPUAdapterType::NumGPUAdapterType; i++)
			{
				if (adaptersByType[i])
				{
					*adapterToUse = adaptersByType[i];
					// �g�p����A�_�v�^�́A���̊֐��̊O�Ŏg���邽�߁A
					// ���̊֐��̍Ō�ɔj������Ă��܂�Ȃ��悤�ɁA�Q�ƃJ�E���^���グ�Ă����B
					(*adapterToUse)->AddRef();
					decided = true;
					break;
				}
			}
			// �D��x�������A�_�v�^���Ȃ���΁A�r�f�I����������ԑ傫���A�_�v�^���g�p����B
			if (decided != true)
			{
				if (maxVideoMemoryAdapter)
				{
					*adapterToUse = maxVideoMemoryAdapter;
					// �g�p����A�_�v�^�́A���̊֐��̊O�Ŏg���邽�߁A
					// ���̊֐��̍Ō�ɔj������Ă��܂�Ȃ��悤�ɁA�Q�ƃJ�E���^���グ�Ă����B
					(*adapterToUse)->AddRef();
				}
			}

			// 3�F���ׂ��A�_�v�^���ׂĂ�j��
			// �g�p����A�_�v�^�́A���O�ɎQ�ƃJ�E���^���グ�Ă���͂��B
			// ������Q�ƃJ�E���^�������Ă��j������Ȃ��͂��B
			for (auto adapter : adapters)
			{
				adapter->Release();
			}

			return;
		}


		bool CGraphicsEngine::CreateCommandAllocator()
		{
			auto result = m_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT, 
				IID_PPV_ARGS(&m_commandAllocator)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�R�}���h�A���P�[�^�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateCommandList(D3D12_COMMAND_LIST_TYPE commandListType)
		{
			auto result = m_device->CreateCommandList(
				0,
				commandListType,
				m_commandAllocator,
				nullptr,
				IID_PPV_ARGS(&m_commandList)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�R�}���h���X�g�̐����Ɏ��s���܂����B");
				return false;
			}

			// �R�}���h���X�g�́A�J����Ă����ԂŐ�������邽�߁A���Ă����B
			m_commandList->Close();

			return true;
		}

		bool CGraphicsEngine::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType)
		{
			D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
			cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cmdQueueDesc.NodeMask = 0;
			cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			// �R�}���h���X�g�ƍ��킹��B
			cmdQueueDesc.Type = commandListType;

			auto result = m_device->CreateCommandQueue(
				&cmdQueueDesc,
				IID_PPV_ARGS(&m_commandQueue)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�R�}���h�L���[�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateSwapChain(IDXGIFactory6* dxgiFactory)
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = CApplication::GetInstance()->GetWindowWidth();
			swapChainDesc.Height = CApplication::GetInstance()->GetWindowHeight();
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = m_kFrameBufferCount;
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
			//fullScreenDesc.RefreshRate.Denominator = 1;
			//fullScreenDesc.RefreshRate.Numerator = 60;
			//fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			//fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			//fullScreenDesc.Windowed = true;

			IDXGISwapChain1* swapChain;
			auto result = dxgiFactory->CreateSwapChainForHwnd(
				m_commandQueue,
				nsGameWindow::CGameWindow::GetInstance()->GetHWND(),
				&swapChainDesc,
				nullptr,
				//&fullScreenDesc,
				nullptr,
				&swapChain
			);

			swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain));
			swapChain->Release();

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�X���b�v�`�F�[���̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateDescriptorHeapForRTV()
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NodeMask = 0;
			heapDesc.NumDescriptors = m_kFrameBufferCount;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			auto result = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"RTV�p�̃f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateRTVForFrameBuffer()
		{
			// �t���[���o�b�t�@�̐���萔�Œ�`���Ă邩��A�X���b�v�`�F�C���̃f�B�X�N���v�^��
			// �擾����K�v�Ȃ��B
			//DXGI_SWAP_CHAIN_DESC swcDesc = {};
			//auto result = m_swapChain->GetDesc(&swcDesc);
			//if (FAILED(result))
			//{
			//	nsGameWindow::MessageBoxError(L"�X���b�v�`�F�[���̎擾�Ɏ��s���܂����B");
			//	return false;
			//}

			// @todo RTV�̃f�B�X�N���v�^��nullptr�i�f�t�H���g�w��j�ł悢�̂����ׂ�B
			// RTV�̃t�H�[�}�b�g���w�肷�邽�߂ɂ���Ǝv���񂾂��ǁA
			// nullptr�Ńf�t�H���g�ݒ�ɂ��Ă���l�������C������B
			//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			////rtvDesc.Format = swcDesc.BufferDesc.Format;
			////rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

			for (int i = 0; static_cast<unsigned int>(i) < m_kFrameBufferCount; i++)
			{
				auto result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"�X���b�v�`�F�C�����̃o�b�t�@�ƃr���[���֘A�t���Ɏ��s���܂����B");
					return false;
				}

				m_device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
				m_renderTargets[i]->SetName(L"FrameBuffer::RenderTargetView");

				rtvHandle.ptr +=
					m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			return true;
		}

		bool CGraphicsEngine::CreateDescriptorHeapForDSV()
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = 1;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			auto result = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"DSV�p�̃f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateDSVForFrameBuffer()
		{
			D3D12_RESOURCE_DESC resDesc = {};
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resDesc.Width = CApplication::GetInstance()->GetWindowWidth();
			resDesc.Height = CApplication::GetInstance()->GetWindowHeight();
			resDesc.DepthOrArraySize = 1;
			resDesc.Format = m_kDepthFormat;
			resDesc.SampleDesc.Count = 1;
			resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

			auto depthClearValue = CD3DX12_CLEAR_VALUE(
				m_kDepthFormat,
				1.0f,
				0
			);

			auto result = m_device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&depthClearValue,
				IID_PPV_ARGS(&m_depthStencilBuffer)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�f�v�X�X�e���V���o�b�t�@�̐����Ɏ��s���܂����B");
				return false;
			}

			// @todo DSV�̃f�B�X�N���v�^��nullptr�i�f�t�H���g�w��j�ł悢�̂����ׂ�B
			// DSV�̃t�H�[�}�b�g���w�肷�邽�߂ɂ���Ǝv���񂾂��ǁA
			// nullptr�Ńf�t�H���g�ݒ�ɂ��Ă���l�������C������B
			//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			//dsvDesc.Format = m_kDepthFormat;
			//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			//dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			m_device->CreateDepthStencilView(
				m_depthStencilBuffer,
				nullptr,
				m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
			);

			return true;
		}

		bool CGraphicsEngine::CreateFence()
		{
			auto result = m_device->CreateFence(
				m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)
			);
			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�t�F���X�̐����Ɏ��s���܂����B");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateSeceneConstantBuff()
		{
			D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			// �K�v�ȃo�C�g����256�ŃA���C�����g�����o�C�g�����K�v�B
			// 0xff = 255
			// �ŏ���0xff�𑫂���256�ȏ�ɂ��Ă���A�r�b�g���]����0xff��AND���Z���邱�ƂŁA
			// size�𒴂����ŏ���256�̔{�����v�Z����B
			UINT64 constBuffSize = (sizeof(SSceneDataMatrix) + 0xff) & ~0xff;
			D3D12_RESOURCE_DESC constBuffResDesc = CD3DX12_RESOURCE_DESC::Buffer(constBuffSize);
			auto result = m_device->CreateCommittedResource(
				&constBuffHeapProp,
				D3D12_HEAP_FLAG_NONE,
				&constBuffResDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_sceneDataConstantBuff)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�V�[���f�[�^�p�̒萔�o�b�t�@�̐����Ɏ��s���܂����B");
				return false;
			}

			result = m_sceneDataConstantBuff->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedSceneDataMatrix));	// �}�b�v
			m_mappedSceneDataMatrix->mView = m_mView;
			m_mappedSceneDataMatrix->mProj = m_mProj;
			m_mappedSceneDataMatrix->cameraPosWS = m_cameraPos;

			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descHeapDesc.NodeMask = 0;
			descHeapDesc.NumDescriptors = 1;
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

			result = m_device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_sceneDataDescriptorHeap));
			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"�V�[���f�[�^�p�̒萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̐����Ɏ��s���܂����B");
				return false;
			}

			auto heapHandle = m_sceneDataDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = m_sceneDataConstantBuff->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = static_cast<UINT>(m_sceneDataConstantBuff->GetDesc().Width);

			m_device->CreateConstantBufferView(&cbvDesc, heapHandle);

			return true;
		}






	}

}