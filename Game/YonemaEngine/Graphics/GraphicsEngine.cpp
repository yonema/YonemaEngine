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
			// 上から使用優先度順
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
				// コマンドリストとコマンドキューで同じタイプを使う。
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

			if (CreateRTVDescriptorHeapForFrameBuffer() != true)
			{
				return false;
			}

			if (CreateRTVForFrameBuffer() != true)
			{
				return false;
			}

			if (CreateDSVDescriptorHeapForFrameBuffer() != true)
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

			if (CreatePeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateRTVDescriptorHeapForPeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateRTVForPeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateSRVDescriptorHeapForPeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateSRVForPeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateVertexBufferForPeraRenderTarget() != true)
			{
				return false;
			}
			if (CreateRootSignatureForPeraRenderTaraget() != true)
			{
				return false;
			}
			if (CreatePipelineStateForPeraRenderTarget() != true)
			{
				return false;
			}
			

			// 初期化が終わったら、DXGIFactoryはもういらないため破棄する。
			dxgiFactory->Release();

			const auto kWindowWidth = CApplication::GetInstance()->GetWindowWidth();
			const auto kWindowHeight = CApplication::GetInstance()->GetWindowHeight();

			m_viewport = CD3DX12_VIEWPORT(m_frameBuffers[0]);
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

			m_cameraPos = {0.0f, 10.0f, -25.0f};
			m_targetPos = { 0.0f, 10.0f, 0.0f };
			m_upDir = { 0.0f, 1.0f, 0.0f };
			m_mView.MakeViewMatrix(m_cameraPos, m_targetPos, m_upDir);
			m_mProj.MakeProjectionMatrix(
				nsMath::DegToRad(60.0f),
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
			if (m_pipelineStateForPeraRT)
			{
				m_pipelineStateForPeraRT->Release();
			}
			if (m_rootSignatureForPeraRT)
			{
				m_rootSignatureForPeraRT->Release();
			}
			if (m_vertexBuffForPeraRT)
			{
				m_vertexBuffForPeraRT->Release();
			}
			if (m_srvDescHeapForPeraRT)
			{
				m_srvDescHeapForPeraRT->Release();
			}
			if (m_rtvDescHeapForPeraRT)
			{
				m_rtvDescHeapForPeraRT->Release();
			}
			if (m_peraRenderTarget)
			{
				m_peraRenderTarget->Release();
			}
			if (m_fence)
			{
				m_fence->Release();
			}
			if (m_depthStencilBuffer)
			{
				m_depthStencilBuffer->Release();
			}
			if (m_dsvDescHeapForFrameBuff)
			{
				m_dsvDescHeapForFrameBuff->Release();
			}
			for (auto& renderTarget : m_frameBuffers)
			{
				if (renderTarget)
				{
					renderTarget->Release();
				}
			}
			if (m_rtvDescHeapForFrameBuff)
			{
				m_rtvDescHeapForFrameBuff->Release();
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
			// ○リセット
			auto result = m_commandAllocator->Reset();
			result = m_commandList->Reset(m_commandAllocator, nullptr);

			//auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();

			//// ○リソースバリア
			//D3D12_RESOURCE_BARRIER barrierDesc =
			//	CD3DX12_RESOURCE_BARRIER::Transition(
			//		m_frameBuffers[bbIdx],
			//		D3D12_RESOURCE_STATE_PRESENT,
			//		D3D12_RESOURCE_STATE_RENDER_TARGET
			//	);
			//m_commandList->ResourceBarrier(1, &barrierDesc);

			D3D12_RESOURCE_BARRIER barrierForPeraRT =
				CD3DX12_RESOURCE_BARRIER::Transition(
					m_peraRenderTarget,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);

			m_commandList->ResourceBarrier(1, &barrierForPeraRT);

			// ○RTVとDSVのセットとクリア
			//auto rtvH = m_rtvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart();
			//rtvH.ptr += static_cast<long long unsigned int>(bbIdx) *
			//	m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto rtvH = m_rtvDescHeapForPeraRT->GetCPUDescriptorHandleForHeapStart();
			auto dsvH = m_dsvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart();

			//m_commandList->OMSetRenderTargets(1, &rtvH, true, &dsvH);
			m_commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

			m_commandList->ClearRenderTargetView(rtvH, m_kRTVClearColor, 0, nullptr);
			m_commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// ○ビューポートとシザー矩形のセット
			m_commandList->RSSetViewports(1, &m_viewport);
			m_commandList->RSSetScissorRects(1, &m_scissorRect);

			// ○描画するモデルに合わせたルートシグネチャとパイプラインステートをセット
			m_commandList->SetGraphicsRootSignature(m_pmdGenericRenderer->GetRootSignature());
			m_commandList->SetPipelineState(m_pmdGenericRenderer->GetPipelineState());

			// ○プリミティブトポロジをセット
			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// ○シーンデータをセット
			m_commandList->SetDescriptorHeaps(1, &m_sceneDataDescriptorHeap);
			auto heapHandle = m_sceneDataDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			m_commandList->SetGraphicsRootDescriptorTable(0, heapHandle);

			return;
		}

		void CGraphicsEngine::EndDraw()
		{



			D3D12_RESOURCE_BARRIER barrierForPeraRT = 
				CD3DX12_RESOURCE_BARRIER::Transition(
				m_peraRenderTarget,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			m_commandList->ResourceBarrier(1, &barrierForPeraRT);



			auto bbIdx = m_swapChain->GetCurrentBackBufferIndex();
			// ○リソースバリア
			D3D12_RESOURCE_BARRIER barrierDesc =
				CD3DX12_RESOURCE_BARRIER::Transition(
					m_frameBuffers[bbIdx],
					D3D12_RESOURCE_STATE_PRESENT,
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);
			m_commandList->ResourceBarrier(1, &barrierDesc);

			auto rtvH = m_rtvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart();
			rtvH.ptr += static_cast<long long unsigned int>(bbIdx) *
				m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto dsvH = m_dsvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart();

			m_commandList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

			m_commandList->ClearRenderTargetView(rtvH, m_kRTVClearColor, 0, nullptr);
			m_commandList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			m_commandList->SetGraphicsRootSignature(m_rootSignatureForPeraRT);
			m_commandList->SetPipelineState(m_pipelineStateForPeraRT);
			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffViewForPeraRT);

			m_commandList->SetDescriptorHeaps(1, &m_srvDescHeapForPeraRT);
			auto heapHandle = m_srvDescHeapForPeraRT->GetGPUDescriptorHandleForHeapStart();
			m_commandList->SetGraphicsRootDescriptorTable(0, heapHandle);

			m_commandList->DrawInstanced(4, 1, 0, 0);


			/*D3D12_RESOURCE_BARRIER*/ barrierDesc =
				CD3DX12_RESOURCE_BARRIER::Transition(
					m_frameBuffers[bbIdx],
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PRESENT
				);
			m_commandList->ResourceBarrier(1, &barrierDesc);

			// 命令の実行前に、必ずコマンドリストのクローズを行う。
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
				nsGameWindow::MessageBoxError(L"DXGIFactoryオブジェクトの生成に失敗しました。");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::EnableDebugLayer()
		{
			ID3D12Debug* debugLayer = nullptr;
			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

			// デバックレイヤーを有効にする。
			debugLayer->EnableDebugLayer();
			// 有効にしたら、インターフェースを開放する。
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
					// 生成可能なバージョンを見つけたら、ループを打ち切り。
					break;
				}
			}

			// デバイス生成が終わったら、アダプタはもう使用しないため、破棄。
			if (adapter)
			{
				adapter->Release();
			}

			if (m_device == nullptr)
			{
				nsGameWindow::MessageBoxError(L"デバイスの生成に失敗しました。");
				return false;
			}

			return true;
		}

		void CGraphicsEngine::FindAdapter(IDXGIFactory6* dxgiFactory, IDXGIAdapter** adapterToUse)
		{
			// 最後にまとめて破棄するために、調べたアダプタすべてを保持しておくリスト。
			std::list<IDXGIAdapter*> adapters;
			// 調べる対象のアダプタ。
			IDXGIAdapter* tempAdapter = nullptr;
			// GPUの種類別のアダプタ。
			// 同じ種類のアダプタがあった場合は、ビデオメモリが大きい方を保持。
			IDXGIAdapter* adaptersByType[EnGPUAdapterType::NumGPUAdapterType] = { nullptr };
			// すべてのアダプタの中で、1番ビデオメモリが大きいアダプタを保持。
			IDXGIAdapter* maxVideoMemoryAdapter = nullptr;
			SIZE_T videoMomorySize = 0;

			// 1：利用可能なアダプタを探す

			// PCにあるアダプタをそれぞれ調べる
			for (int i = 0; dxgiFactory->EnumAdapters(i, &tempAdapter) != DXGI_ERROR_NOT_FOUND; i++)
			{
				adapters.emplace_back(tempAdapter);
				DXGI_ADAPTER_DESC tempAdapterDesc = {};
				tempAdapter->GetDesc(&tempAdapterDesc);

				// 優先度の高いアダプタがあるか調べる
				for (int adapterType = 0; adapterType < EnGPUAdapterType::NumGPUAdapterType; adapterType++)
				{
					std::wstring strDesc = tempAdapterDesc.Description;
					if (strDesc.find(m_kGPUAdapterNames[adapterType]) == std::string::npos)
					{
						// 種類の名前が違う、次の種類へ。
						continue;
					}

					if (adaptersByType[adapterType])
					{
						// 同じ種類のアダプタが複数見つかった。
						// ビデオメモリが大きいほうを使用する。

						DXGI_ADAPTER_DESC adaptersDesc;
						adaptersByType[adapterType]->GetDesc(&adaptersDesc);
						if (tempAdapterDesc.DedicatedVideoMemory <= adaptersDesc.DedicatedVideoMemory)
						{
							// ビデオメモリが少ないから、使用しない。
							break;
						}
					}

					adaptersByType[adapterType] = tempAdapter;
						
					break;
					
				}


				// ビデオメモリが一番大きいアダプタを探す
				if (tempAdapterDesc.DedicatedVideoMemory > videoMomorySize)
				{
					maxVideoMemoryAdapter = tempAdapter;
					videoMomorySize = tempAdapterDesc.DedicatedVideoMemory;
				}
			}


			// 2：探し出したアダプタから、どのアダプタを使用するか選ぶ

			bool decided = false;
			// 優先度の高いアダプタから順に調べる
			for (int i = 0; i < EnGPUAdapterType::NumGPUAdapterType; i++)
			{
				if (adaptersByType[i])
				{
					*adapterToUse = adaptersByType[i];
					// 使用するアダプタは、この関数の外で使われるため、
					// この関数の最後に破棄されてしまわないように、参照カウンタを上げておく。
					(*adapterToUse)->AddRef();
					decided = true;
					break;
				}
			}
			// 優先度が高いアダプタがなければ、ビデオメモリが一番大きいアダプタを使用する。
			if (decided != true)
			{
				if (maxVideoMemoryAdapter)
				{
					*adapterToUse = maxVideoMemoryAdapter;
					// 使用するアダプタは、この関数の外で使われるため、
					// この関数の最後に破棄されてしまわないように、参照カウンタを上げておく。
					(*adapterToUse)->AddRef();
				}
			}

			// 3：調べたアダプタすべてを破棄
			// 使用するアダプタは、事前に参照カウンタを上げてあるはず。
			// だから参照カウンタを下げても破棄されないはず。
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
				nsGameWindow::MessageBoxError(L"コマンドアロケータの生成に失敗しました。");
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
				nsGameWindow::MessageBoxError(L"コマンドリストの生成に失敗しました。");
				return false;
			}

			// コマンドリストは、開かれている状態で生成されるため、閉じておく。
			m_commandList->Close();

			return true;
		}

		bool CGraphicsEngine::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType)
		{
			D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
			cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cmdQueueDesc.NodeMask = 0;
			cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			// コマンドリストと合わせる。
			cmdQueueDesc.Type = commandListType;

			auto result = m_device->CreateCommandQueue(
				&cmdQueueDesc,
				IID_PPV_ARGS(&m_commandQueue)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"コマンドキューの生成に失敗しました。");
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
				nsGameWindow::MessageBoxError(L"スワップチェーンの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateRTVDescriptorHeapForFrameBuffer()
		{
			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			descHeapDesc.NodeMask = 0;
			descHeapDesc.NumDescriptors = m_kFrameBufferCount;
			descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			auto result = m_device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_rtvDescHeapForFrameBuff));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"フレームバッファ用のRTVディスクリプタヒープの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateRTVForFrameBuffer()
		{
			// フレームバッファの数を定数で定義してるから、スワップチェインのディスクリプタを
			// 取得する必要なし。
			//DXGI_SWAP_CHAIN_DESC swcDesc = {};
			//auto result = m_swapChain->GetDesc(&swcDesc);
			//if (FAILED(result))
			//{
			//	nsGameWindow::MessageBoxError(L"スワップチェーンの取得に失敗しました。");
			//	return false;
			//}

			// @todo RTVのディスクリプタがnullptr（デフォルト指定）でよいのか調べる。
			// RTVのフォーマットを指定するためにいると思うんだけど、
			// nullptrでデフォルト設定にしている人が多い気がする。
			//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			////rtvDesc.Format = swcDesc.BufferDesc.Format;
			////rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = 
				m_rtvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart();

			for (int i = 0; static_cast<unsigned int>(i) < m_kFrameBufferCount; i++)
			{
				auto result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_frameBuffers[i]));
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"スワップチェイン内のバッファとビューを関連付けに失敗しました。");
					return false;
				}

				m_device->CreateRenderTargetView(m_frameBuffers[i], nullptr, rtvHandle);
				m_frameBuffers[i]->SetName(L"FrameBuffer::RenderTargetView");

				rtvHandle.ptr +=
					m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			return true;
		}

		bool CGraphicsEngine::CreateDSVDescriptorHeapForFrameBuffer()
		{
			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.NumDescriptors = 1;
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			auto result = m_device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_dsvDescHeapForFrameBuff));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"フレームバッファ用のDSVディスクリプタヒープの生成に失敗しました。");
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
				nsGameWindow::MessageBoxError(L"デプスステンシルバッファの生成に失敗しました。");
				return false;
			}

			// @todo DSVのディスクリプタがnullptr（デフォルト指定）でよいのか調べる。
			// DSVのフォーマットを指定するためにいると思うんだけど、
			// nullptrでデフォルト設定にしている人が多い気がする。
			//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			//dsvDesc.Format = m_kDepthFormat;
			//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			//dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			m_device->CreateDepthStencilView(
				m_depthStencilBuffer,
				nullptr,
				m_dsvDescHeapForFrameBuff->GetCPUDescriptorHandleForHeapStart()
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
				nsGameWindow::MessageBoxError(L"フェンスの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreatePeraRenderTarget()
		{
			auto rtvDescHeapDesc = m_rtvDescHeapForFrameBuff->GetDesc();
			auto& frameBuff = m_frameBuffers[0];
			auto frameBuffResDesc = frameBuff->GetDesc();

			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(
				DXGI_FORMAT_R8G8B8A8_UNORM, m_kRTVClearColor);

			auto result = m_device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&frameBuffResDesc,
				// D3D12_RESOURCE_STATE_RENDER_TARGETではない。
				// 後でステートを切り替えるまでは、テクスチャとして扱う。
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				&clearValue,
				IID_PPV_ARGS(&m_peraRenderTarget)
			);
			

			return true;
		}

		bool CGraphicsEngine::CreateRTVDescriptorHeapForPeraRenderTarget()
		{
			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			descHeapDesc.NodeMask = 0;
			descHeapDesc.NumDescriptors = 1;
			descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			auto result = m_device->CreateDescriptorHeap(
				&descHeapDesc, IID_PPV_ARGS(&m_rtvDescHeapForPeraRT));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"ペラレンダーターゲット用のRTVディスクリプタヒープの生成に失敗しました。");
				return false;
			}
			return true;
		}

		bool CGraphicsEngine::CreateRTVForPeraRenderTarget()
		{
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
				m_rtvDescHeapForPeraRT->GetCPUDescriptorHandleForHeapStart();

			m_device->CreateRenderTargetView(m_peraRenderTarget, &rtvDesc, rtvHandle);
			m_peraRenderTarget->SetName(L"PeraRenderTarget::RenderTargetView");

			return true;
		}

		bool CGraphicsEngine::CreateSRVDescriptorHeapForPeraRenderTarget()
		{
			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descHeapDesc.NodeMask = 0;
			descHeapDesc.NumDescriptors = 1;
			descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			auto result = m_device->CreateDescriptorHeap(
				&descHeapDesc, IID_PPV_ARGS(&m_srvDescHeapForPeraRT));

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"ペラレンダーターゲット用のSRVディスクリプタヒープの生成に失敗しました。");
				return false;
			}

			return true;
		}

		bool CGraphicsEngine::CreateSRVForPeraRenderTarget()
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			D3D12_CPU_DESCRIPTOR_HANDLE srvHandle =
				m_srvDescHeapForPeraRT->GetCPUDescriptorHandleForHeapStart();

			m_device->CreateShaderResourceView(m_peraRenderTarget, &srvDesc, srvHandle);


			return true;
		}

		bool CGraphicsEngine::CreateVertexBufferForPeraRenderTarget()
		{
			constexpr SPeraVertex peraVertices[4] =
			{
				{ { -1.0f, -1.0f, 0.1f }, { 0.0f, 1.0f } },	// 左下
				{ { -1.0f,  1.0f, 0.1f }, { 0.0f, 0.0f } },	// 左上
				{ {  1.0f, -1.0f, 0.1f }, { 1.0f, 1.0f } },	// 右下
				{ {  1.0f,  1.0f, 0.1f }, { 1.0f, 0.0f } }	// 右上
			};

			D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(peraVertices));


			// ○頂点バッファの作成
			auto result = m_device->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_vertexBuffForPeraRT)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxWarning(L"ペラレンダーターゲット用の頂点バッファの生成に失敗しました。");
				return false;
			}

			// ○頂点バッファビューの作成
			m_vertexBuffViewForPeraRT.BufferLocation = 
				m_vertexBuffForPeraRT->GetGPUVirtualAddress();
			m_vertexBuffViewForPeraRT.SizeInBytes = sizeof(peraVertices);
			m_vertexBuffViewForPeraRT.StrideInBytes = sizeof(SPeraVertex);

			// 〇頂点情報のコピー（マップ）
			SPeraVertex* mappedPeraVertices = nullptr;
			m_vertexBuffForPeraRT->Map(0, nullptr, reinterpret_cast<void**>(&mappedPeraVertices));
			std::copy(std::begin(peraVertices), std::end(peraVertices), mappedPeraVertices);
			m_vertexBuffForPeraRT->Unmap(0, nullptr);

			return true;
		}

		bool CGraphicsEngine::CreateRootSignatureForPeraRenderTaraget()
		{
			constexpr int unsigned kNumDescTableRanges = 1;
			constexpr int unsigned kNumRootParameters = 1;
			constexpr int unsigned kNumSamplers = 1;

			CD3DX12_DESCRIPTOR_RANGE1 descTableRanges[kNumDescTableRanges] = {};
			descTableRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

			CD3DX12_ROOT_PARAMETER1 rootParameters[kNumRootParameters] = {};
			rootParameters[0].InitAsDescriptorTable(
				kNumDescTableRanges, descTableRanges, D3D12_SHADER_VISIBILITY_PIXEL);

			CD3DX12_STATIC_SAMPLER_DESC samplerDescs[kNumSamplers] = {};
			samplerDescs[0].Init(
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

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(
				kNumRootParameters,
				rootParameters,
				kNumSamplers,
				samplerDescs,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
			);


			ID3DBlob* rootSignatureBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;

			auto result = D3DX12SerializeVersionedRootSignature
			(
				&rootSignatureDesc,
				D3D_ROOT_SIGNATURE_VERSION_1_1,
				&rootSignatureBlob,
				&errorBlob
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxWarning(L"ペラレンダーターゲット用のルートシグネチャのバイナリコードの生成に失敗しました。");
				return false;
			}

			result = m_device->CreateRootSignature(
				0,
				rootSignatureBlob->GetBufferPointer(),
				rootSignatureBlob->GetBufferSize(),
				IID_PPV_ARGS(&m_rootSignatureForPeraRT)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxWarning(L"ペラレンダーターゲット用のルートシグネチャの生成に失敗しました。");
				return false;
			}

			if (errorBlob)
			{
				nsGameWindow::MessageBoxWarning(L"ペラレンダーターゲット用のルートシグネチャの生成に失敗しました。");
				errorBlob->Release();
				return false;
			}

			// 不要になったため解放。
			rootSignatureBlob->Release();

			return true;
		}

		bool CGraphicsEngine::CreatePipelineStateForPeraRenderTarget()
		{
			ID3DBlob* vsBlob = nullptr;
			ID3DBlob* psBlob = nullptr;
			ID3DBlob* errorBlob = nullptr;

			auto result = D3DCompileFromFile
			(
				L"Assets/Shaders/PeraPolygonVertexShader.hlsl",
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"VSMain",
				"vs_5_0",
				0,
				0,
				&vsBlob,
				&errorBlob
			);
			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"ペラレンダーターゲット用の頂点シェーダーの生成に失敗しました。");

				if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				{
					::OutputDebugStringA("ファイルが見当たりません");
				}
				else
				{
					std::string errstr;
					errstr.resize(errorBlob->GetBufferSize());
					std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
					errstr += "\n";
					::OutputDebugStringA(errstr.c_str());
				}

				if (errorBlob)
				{
					errorBlob->Release();
				}
				return false;
			}
			else
			{
				if (errorBlob)
				{
					errorBlob->Release();
				}
			}
		

			result = D3DCompileFromFile
			(
				L"Assets/Shaders/PeraPolygonPixelShader.hlsl",
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"PSMain",
				"ps_5_0",
				0,
				0,
				&psBlob,
				&errorBlob
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxError(L"ペラレンダーターゲット用のピクセルシェーダーの生成に失敗しました。");

				if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				{
					::OutputDebugStringA("ファイルが見当たりません");
				}
				else
				{
					std::string errstr;
					errstr.resize(errorBlob->GetBufferSize());
					std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
					errstr += "\n";
					::OutputDebugStringA(errstr.c_str());
				}

				if (errorBlob)
				{
					errorBlob->Release();
				}
				return false;
			}
			else
			{
				if (errorBlob)
				{
					errorBlob->Release();
				}
			}


			D3D12_INPUT_ELEMENT_DESC layout[] =
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

			D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipelineStateDesc = {};
			gPipelineStateDesc.InputLayout.NumElements = _countof(layout);
			gPipelineStateDesc.InputLayout.pInputElementDescs = layout;

			gPipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob);
			gPipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob);

			gPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			gPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			gPipelineStateDesc.NumRenderTargets = 1;
			gPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			gPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			gPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
			gPipelineStateDesc.SampleDesc.Count = 1;
			gPipelineStateDesc.SampleDesc.Quality = 0;
			gPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			gPipelineStateDesc.pRootSignature = m_rootSignatureForPeraRT;

			result = m_device->CreateGraphicsPipelineState(
				&gPipelineStateDesc,
				IID_PPV_ARGS(&m_pipelineStateForPeraRT)
			);

			if (FAILED(result))
			{
				nsGameWindow::MessageBoxWarning(L"ペラレンダーターゲット用のグラフィックスパイプラインステートの生成に失敗しました。");
				return false;
			}



			return true;
		}

		bool CGraphicsEngine::CreateSeceneConstantBuff()
		{
			D3D12_HEAP_PROPERTIES constBuffHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			// 必要なバイト数を256でアライメントしたバイト数が必要。
			// 0xff = 255
			// 最初に0xffを足して256以上にしてから、ビット反転した0xffとAND演算することで、
			// sizeを超えた最小の256の倍数を計算する。
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
				nsGameWindow::MessageBoxError(L"シーンデータ用の定数バッファの生成に失敗しました。");
				return false;
			}

			result = m_sceneDataConstantBuff->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedSceneDataMatrix));	// マップ
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
				nsGameWindow::MessageBoxError(L"シーンデータ用の定数バッファのディスクリプタヒープの生成に失敗しました。");
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