#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CRenderTarget : private nsUtils::SNoncopyable
			{
			public:
				constexpr CRenderTarget() = default;
				~CRenderTarget() = default;

				void Init();

				void Release();

			private:
				ID3D12Resource* m_renderTarget = nullptr;
				ID3D12Resource* m_depthStencilBuffer = nullptr;
				CDescriptorHeap m_rtvDescriptorHeap;
				CDescriptorHeap m_dsvDescriptorHeap;
			};

		}
	}
}