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
				static const nsMath::CVector4 m_kRTVClearColor;
				static const float m_kDsvClearValue;


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

				constexpr auto GetFrameBufferCount() const noexcept
				{
					return m_kFrameBufferCount;
				}

				constexpr auto GetCurrentRenerTarget() const noexcept
				{
					return m_renderTargets[m_backBufferIndex];
				}
				constexpr auto GetCurrentRtvCpuDescriptorHandle() const noexcept
				{
					return m_currentRtvCpuDescriptorHandle;
				}
				constexpr auto GetDsvCpuDescriptorHandle() const noexcept
				{
					return m_dsvCpuDescriptorHandle;
				}

				constexpr auto GetCurrentBackBufferIndex() const noexcept
				{
					return  m_backBufferIndex;
				}

				constexpr const auto& GetRtvClearColor() const noexcept
				{
					return m_kRTVClearColor;
				}
				constexpr const float GetDsvClearValue() const noexcept
				{
					return m_kDsvClearValue;
				}

				auto GetResourceDesc() const noexcept
				{
					return m_renderTargets[0]->GetDesc();
				}

				constexpr const auto& GetViewport() const noexcept
				{
					return m_viewport;
				}
				constexpr const auto& GetScissorRect() const noexcept
				{
					return m_scissorRect;
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
				D3D12_CPU_DESCRIPTOR_HANDLE m_currentRtvCpuDescriptorHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE m_dsvCpuDescriptorHandle = {};
				D3D12_VIEWPORT m_viewport = {};
				D3D12_RECT m_scissorRect = {};

			};

		}
	}
}