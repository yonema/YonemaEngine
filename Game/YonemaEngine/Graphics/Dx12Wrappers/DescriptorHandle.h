#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CDescriptorHandle : private nsUtils::SNoncopyable
			{
			public:
				constexpr CDescriptorHandle() = default;
				~CDescriptorHandle() = default;

				void Init(
					unsigned int size,
					D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleStart,
					D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleStart,
					D3D12_DESCRIPTOR_HEAP_TYPE descHeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
				) noexcept;

				inline auto GetCpuHandle(unsigned int index) const noexcept
				{
					return m_cpuHandle[index];
				}

				inline auto GetGpuHandle(unsigned int index) const noexcept
				{
					return m_gpuHandle[index];
				}
				

			private:
				std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_cpuHandle = {};
				std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> m_gpuHandle = {};
			};

		}
	}
}