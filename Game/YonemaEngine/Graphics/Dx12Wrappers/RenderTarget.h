#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{

		class CRenderTarget : private nsUtils::SNoncopyable
		{
		public:
			CRenderTarget() = default;
			~CRenderTarget() = default;

		private:
			ID3D12Resource* m_renderTarget = nullptr;
			ID3D12DescriptorHeap* m_rtvDescHeap = nullptr;
			ID3D12DescriptorHeap* m_srvDescHeap = nullptr;
		};

	}
}