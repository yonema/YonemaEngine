#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CDescriptorHeap : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CDescriptorHeap() = default;
				~CDescriptorHeap();

				bool Init(
					D3D12_DESCRIPTOR_HEAP_TYPE type,
					UINT numDescriptors,
					D3D12_DESCRIPTOR_HEAP_FLAGS flags,
					const wchar_t* objectName = nullptr
				);

				bool InitAsCbvSrvUav(
					UINT numDescriptors,
					const wchar_t* objectName = nullptr,
					D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
				);
				bool InitAsRTV(UINT numDescriptors = 1, const wchar_t* objectName = nullptr);
				bool InitAsDSV(const wchar_t* objectName = nullptr);

				void Release();

				inline auto GetCPUHandle() const
				{
					return m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
				}

				inline auto GetGPUHandle() const
				{
					return m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
				}

				constexpr auto Get() const noexcept
				{
					return m_descriptorHeap;
				}

				constexpr bool IsValid() const noexcept
				{
					return m_descriptorHeap ? true : false;
				}

				void SetName(const wchar_t* objectName);
				

			private:
				void Terminate();



			private:
				ID3D12DescriptorHeap* m_descriptorHeap = nullptr;
			};

		}
	}
}