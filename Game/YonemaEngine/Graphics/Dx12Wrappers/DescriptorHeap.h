#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CDescriptorHeap : private nsUtils::SNoncopyable
			{
			public:
				constexpr CDescriptorHeap() = default;
				~CDescriptorHeap();

				bool Init(
					D3D12_DESCRIPTOR_HEAP_TYPE type, 
					UINT numDescHeaps, 
					D3D12_DESCRIPTOR_HEAP_FLAGS flags
				);

				bool InitAsCbvSrvUav(
					UINT numDescHeaps,
					D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
				);
				bool InitAsRTV(UINT numDescHeaps = 1);
				bool InitAsDSV();

				void Release();

				inline auto GetCPUHandle()
				{
					return m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
				}

				inline auto GetGPUHandle()
				{
					return m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
				}

				constexpr auto Get() noexcept
				{
					return m_descriptorHeap;
				}
				

			private:
				void Terminate();



			private:
				ID3D12DescriptorHeap* m_descriptorHeap = nullptr;
			};

		}
	}
}