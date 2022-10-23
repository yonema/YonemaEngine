#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CFrameBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const unsigned int m_kFrameBufferCount = 2;
				static const DXGI_FORMAT m_kDepthFormat;
				static const float m_kRTVClearColor[4];


			public:
				constexpr CFrameBuffer() = default;
				~CFrameBuffer();

				bool Init(
					ID3D12Device5* device,
					IDXGIFactory6* dxgiFactory,
					ID3D12CommandQueue* commandQueue, 
					HWND hwnd,
					UINT frameBufferWidth,
					UINT frameBufferHeight
					);

				void Release();

				constexpr auto GetRtvCpuDescriptorHandle() noexcept
				{
					return m_rtvCpuDescriptorHandle;
				}
				constexpr auto GetDsvCpuDescriptorHandle() noexcept
				{
					return m_dsvCpuDescriptorHandle;
				}

				constexpr auto GetCurrentBackBufferIndex() noexcept
				{
					return  m_backBufferIndex;
				}

				constexpr auto GetRtvClearColor() noexcept
				{
					return m_kRTVClearColor;
				}

				auto GetResourceDesc() noexcept
				{
					return m_renderTargets[0]->GetDesc();
				}

				void SwapBackBuffer();

				void TransitionFromPresentToRenderTarget(ID3D12GraphicsCommandList* commandList);

				void SetRenderTarget(ID3D12GraphicsCommandList* commandList);

				void SetViewportAndScissorRect(ID3D12GraphicsCommandList* commandList);

				void ClearRenderTargetView(ID3D12GraphicsCommandList* commandList);

				void ClearDepthStencilView(ID3D12GraphicsCommandList* commandList);

				void TransitionFromRenderTargetToPresent(ID3D12GraphicsCommandList* commandList);

				void Present();



			private:
				void Terminate();

				bool CreateSwapChain(
					IDXGIFactory6* dxgiFactory,
					ID3D12CommandQueue* commandQueue,
					HWND hwnd,
					UINT frameBufferWidth,
					UINT frameBufferHeight
				);

				bool CreateDescriptorHeap();

				bool CreateRenderTargets(ID3D12Device5* device);

				bool CreateDepthStencilBuffer(
					ID3D12Device5* device, UINT frameBufferWidth, UINT frameBufferHeight);

				void InitViewportAndScissorRect(
					UINT frameBufferWidth, UINT frameBufferHeight) noexcept;


			private:
				IDXGISwapChain4* m_swapChain = nullptr;
				unsigned int m_backBufferIndex = 0;
				ID3D12Resource* m_renderTargets[m_kFrameBufferCount] = { nullptr };
				ID3D12Resource* m_depthStencilBuffer = nullptr;
				CDescriptorHeap m_rtvDescriptorHeap;
				CDescriptorHeap m_dsvDescriptorHeap;
				D3D12_CPU_DESCRIPTOR_HANDLE m_rtvCpuDescriptorHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE m_dsvCpuDescriptorHandle = {};
				D3D12_VIEWPORT m_viewport = {};
				D3D12_RECT m_scissorRect = {};

			};

		}
	}
}