#include "FrameBuffer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const bool CFrameBuffer::m_kEnableVerticalSync = true;



			const DXGI_FORMAT CFrameBuffer::m_kDepthFormat = DXGI_FORMAT_D32_FLOAT;
			const nsMath::CVector4 CFrameBuffer::m_kRTVClearColor = nsMath::CVector4::Gray();
			const float CFrameBuffer::m_kDsvClearValue = 1.0f;



			CFrameBuffer::~CFrameBuffer()
			{
				Terminate();
				return;
			}

			void CFrameBuffer::Terminate()
			{
				Release();
				return;
			}

			void CFrameBuffer::Release()
			{
				if (m_depthStencilBuffer)
				{
					m_depthStencilBuffer->Release();
					m_depthStencilBuffer = nullptr;
				}
				for (auto& renderTarget : m_renderTargets)
				{
					if (renderTarget)
					{
						renderTarget->Release();
						renderTarget = nullptr;
					}
				}
				m_dsvDescriptorHeap.Release();
				m_rtvDescriptorHeap.Release();
				if (m_swapChain)
				{
					m_swapChain->Release();
					m_swapChain = nullptr;
				}

				return;
			}

			bool CFrameBuffer::Init(
				ID3D12Device5* device,
				IDXGIFactory6* dxgiFactory,
				ID3D12CommandQueue* commandQueue,
				HWND hwnd,
				UINT frameBufferWidth,
				UINT frameBufferHeight
			)
			{
				Release();

				if (CreateSwapChain(
					dxgiFactory, commandQueue, hwnd, frameBufferWidth, frameBufferHeight
				) != true)
				{
					return false;
				}

				if (CreateDescriptorHeap() != true)
				{
					return false;
				}

				if (CreateRenderTargets(device) != true)
				{
					return false;
				}

				if (CreateDepthStencilBuffer(device, frameBufferWidth, frameBufferHeight)
					!= true)
				{
					return false;
				}

				InitViewportAndScissorRect(frameBufferWidth, frameBufferHeight);

				//m_rtvCpuDescriptorHandle = m_rtvDescriptorHeap.GetCPUHandle();
				SwapBackBuffer();
				m_dsvCpuDescriptorHandle = m_dsvDescriptorHeap.GetCPUHandle();

				if (m_kEnableVerticalSync)
				{
					// 垂直同期あり
					m_syncInterval = 1;
				}
				else
				{
					// 垂直同期なし
					m_syncInterval = 0;
				}

				return true;
			}

			bool CFrameBuffer::CreateSwapChain(
				IDXGIFactory6* dxgiFactory,
				ID3D12CommandQueue* commandQueue,
				HWND hwnd,
				UINT frameBufferWidth,
				UINT frameBufferHeight
			)
			{
				DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
				swapChainDesc.Width = frameBufferWidth;
				swapChainDesc.Height = frameBufferHeight;
				swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.Stereo = false;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = m_kFrameBufferCount;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

				// フルスクリーンは非対応
				//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

				//DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
				//fullScreenDesc.RefreshRate.Denominator = 1;
				//fullScreenDesc.RefreshRate.Numerator = 60;
				//fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				//fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				//fullScreenDesc.Windowed = true;

				// CreateSwapChainForHwnd()関数がIDXGISwapChain1のインターフェースしか
				// 対応していないため、いったんIDXGISwapChain1で作成する。
				IDXGISwapChain1* swapChain;
				auto result = dxgiFactory->CreateSwapChainForHwnd(
					commandQueue,
					hwnd,
					&swapChainDesc,
					nullptr,
					//&fullScreenDesc,
					nullptr,
					&swapChain
				);
				// IDXGISwapChain1から、使用するバージョンのインターフェースに変更する。
				swapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain));
				// 元の奴は、もういらないため破棄。
				swapChain->Release();

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"スワップチェーンの生成に失敗しました。");
					return false;
				}

				return true;
			}

			bool CFrameBuffer::CreateDescriptorHeap()
			{
				if (m_rtvDescriptorHeap.InitAsRTV(m_kFrameBufferCount, L"FrameBuffer::RenderTargetView")
					!= true)
				{
					return false;
				}
				if (m_dsvDescriptorHeap.InitAsDSV(L"FrameBuffer::DepthStencilBuffer")
					!= true)
				{
					return false;
				}

				return true;
			}

			bool CFrameBuffer::CreateRenderTargets(ID3D12Device5* device)
			{
				DXGI_SWAP_CHAIN_DESC swcDesc = {};
				auto result = m_swapChain->GetDesc(&swcDesc);
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"スワップチェーンの取得に失敗しました。");
					return false;
				}

				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
				// スワップチェーンとフォーマットを合わせる。
				rtvDesc.Format = swcDesc.BufferDesc.Format;
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

				auto descHeapHandle = m_rtvDescriptorHeap.GetCPUHandle();
				const auto descSizeOfRtv =
					CGraphicsEngine::GetInstance()->GetDescriptorSizeOfRtv();

				for (unsigned int i = 0; i < m_kFrameBufferCount; i++)
				{
					result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
					if (FAILED(result))
					{
						nsGameWindow::MessageBoxError(L"スワップチェイン内のバッファとビューを関連付けに失敗しました。");
						return false;
					}

					device->CreateRenderTargetView(m_renderTargets[i], &rtvDesc, descHeapHandle);
					//m_renderTargets[i]->SetName(L"FrameBuffer::RenderTargetView");
					m_renderTargets[i]->SetName(L"RenderTarget : FrameBuffer::RenderTargetView");

					descHeapHandle.ptr += descSizeOfRtv;

				}

				return true;
			}

			bool CFrameBuffer::CreateDepthStencilBuffer(
				ID3D12Device5* device, UINT frameBufferWidth, UINT frameBufferHeight)
			{
				D3D12_RESOURCE_DESC resDesc = {};
				resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				resDesc.Width = frameBufferWidth;
				resDesc.Height = frameBufferHeight;
				resDesc.DepthOrArraySize = 1;
				resDesc.Format = m_kDepthFormat;
				resDesc.SampleDesc.Count = 1;
				resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

				auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

				auto depthClearValue = CD3DX12_CLEAR_VALUE(
					m_kDepthFormat,
					m_kDsvClearValue,
					0
				);

				auto result = device->CreateCommittedResource(
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

				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = m_kDepthFormat;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

				const auto descHeapHandle = m_dsvDescriptorHeap.GetCPUHandle();

				device->CreateDepthStencilView(m_depthStencilBuffer, &dsvDesc, descHeapHandle);

				return true;
			}

			void CFrameBuffer::InitViewportAndScissorRect(
				UINT frameBufferWidth, UINT frameBufferHeight) noexcept
			{
				m_viewport = CD3DX12_VIEWPORT(m_renderTargets[0]);

				m_scissorRect.left = 0;
				m_scissorRect.top = 0;
				m_scissorRect.right = m_scissorRect.left + frameBufferWidth;
				m_scissorRect.bottom = m_scissorRect.top + frameBufferHeight;

				return;
			}

			void CFrameBuffer::SwapBackBuffer()
			{
				m_backBufferIndex =  m_swapChain->GetCurrentBackBufferIndex();
				m_currentRtvCpuDescriptorHandle = m_rtvDescriptorHeap.GetCPUHandle();
				m_currentRtvCpuDescriptorHandle.ptr += m_backBufferIndex *
					CGraphicsEngine::GetInstance()->GetDescriptorSizeOfRtv();

				return;
			}

			void CFrameBuffer::TransitionFromPresentToRenderTarget(
				ID3D12GraphicsCommandList* commandList)
			{
				// ○リソースバリア
				D3D12_RESOURCE_BARRIER barrierDesc =
					CD3DX12_RESOURCE_BARRIER::Transition(
						m_renderTargets[m_backBufferIndex],
						D3D12_RESOURCE_STATE_PRESENT,
						D3D12_RESOURCE_STATE_RENDER_TARGET
					);
				commandList->ResourceBarrier(1, &barrierDesc);

				return;
			}

			void CFrameBuffer::SetRenderTarget(ID3D12GraphicsCommandList* commandList)
			{
				auto rtvH = m_rtvDescriptorHeap.GetCPUHandle();
				rtvH.ptr += m_backBufferIndex *
					CGraphicsEngine::GetInstance()->GetDescriptorSizeOfRtv();

				commandList->OMSetRenderTargets(1, &rtvH, false, &m_dsvCpuDescriptorHandle);

				return;
			}

			void CFrameBuffer::SetViewportAndScissorRect(ID3D12GraphicsCommandList* commandList)
			{
				commandList->RSSetViewports(1, &m_viewport);
				commandList->RSSetScissorRects(1, &m_scissorRect);

				return;
			}
			void CFrameBuffer::ClearRenderTargetView(ID3D12GraphicsCommandList* commandList)
			{
				commandList->ClearRenderTargetView(
					m_currentRtvCpuDescriptorHandle, m_kRTVClearColor.m_fVec, 0, nullptr);

				return;
			}

			void CFrameBuffer::ClearDepthStencilView(ID3D12GraphicsCommandList* commandList)
			{
				commandList->ClearDepthStencilView(
					m_dsvCpuDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, m_kDsvClearValue, 0, 0, nullptr);

				return;
			}

			void CFrameBuffer::TransitionFromRenderTargetToPresent(
				ID3D12GraphicsCommandList* commandList)
			{
				D3D12_RESOURCE_BARRIER barrierDesc =
					CD3DX12_RESOURCE_BARRIER::Transition(
						m_renderTargets[m_backBufferIndex],
						D3D12_RESOURCE_STATE_RENDER_TARGET,
						D3D12_RESOURCE_STATE_PRESENT
					);
				commandList->ResourceBarrier(1, &barrierDesc);

				return;
			}

			void CFrameBuffer::Present()
			{
				m_swapChain->Present(m_syncInterval, 0);

				return;
			}

		}
	}
}