#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CRenderTarget : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CRenderTarget() = default;
				~CRenderTarget();

				bool Init(
					unsigned int width,
					unsigned int height, 
					DXGI_FORMAT colorFormat,
					const nsMath::CVector4& clearColor = nsMath::CVector4::Black(),
					DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN,
					D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					const wchar_t* objectName = nullptr
					);

				void Release();

				constexpr bool IsValidDepthStencilBuffer() const noexcept
				{
					return m_depthStencilBuffer;
				}

				constexpr const auto Get() const noexcept
				{
					return m_renderTarget;
				}
				constexpr auto* GetRenderTargetTexture() noexcept
				{
					return &m_renderTargetTexture;
				}

				inline auto GetRtvDescriptorCpuHandle() const
				{
					return m_rtvDescriptorHeap.GetCPUHandle();
				}

				inline auto GetDsvDescriptorCpuHandle() const
				{
					return m_dsvDescriptorHeap.GetCPUHandle();
				}

				constexpr const auto& GetRtvClearColor() const noexcept
				{
					return m_rtvClearColor;
				}
				constexpr auto GetDsvClearValue() const noexcept
				{
					return m_dsvClearValue;
				}

				constexpr unsigned int GetWidth() const noexcept
				{
					return m_width;
				}
				constexpr unsigned int GetHeight() const noexcept
				{
					return m_height;
				}

				void SetName(const wchar_t* objectName);

			private:
				void Terminate();

				bool CreateRenderTarget(
					ID3D12Device5* device,
					unsigned int width,
					unsigned int height,
					DXGI_FORMAT colorFormat,
					const nsMath::CVector4& clearColor,
					D3D12_RESOURCE_STATES initialResourceState
					);

				void CreateRenderTargetView(ID3D12Device5* device, DXGI_FORMAT colorFormat);

				bool CreateDepthStencilBuffer(
					ID3D12Device5* device,
					unsigned int width,
					unsigned int height,
					DXGI_FORMAT depthStencilFormat
					);

				void CreateDepthStencilView(ID3D12Device5* device, DXGI_FORMAT depthStencilFormat);

			private:
				ID3D12Resource* m_renderTarget = nullptr;
				ID3D12Resource* m_depthStencilBuffer = nullptr;
				CDescriptorHeap m_rtvDescriptorHeap;
				CDescriptorHeap m_dsvDescriptorHeap;
				CTexture m_renderTargetTexture;
				nsMath::CVector4 m_rtvClearColor = nsMath::CVector4::White();
				float m_dsvClearValue = 1.0f;

				unsigned int m_width = 0;
				unsigned int m_height = 0;
			};

		}
	}
}