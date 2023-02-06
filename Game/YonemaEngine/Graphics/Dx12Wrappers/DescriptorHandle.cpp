#include "DescriptorHandle.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			void CDescriptorHandle::Init(
				unsigned int size,
				D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleStart,
				D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleStart,
				D3D12_DESCRIPTOR_HEAP_TYPE descHeapType
			) noexcept
			{
				unsigned int inc = 0;
				switch (descHeapType)
				{
				case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
					inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfCbvSrvUav();
					break;
				case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
					inc = CGraphicsEngine::GetInstance()->GetDescriptorSizeOfRtv();
					break;
				default:
					break;
				}
					

				m_cpuHandle.resize(size);
				m_gpuHandle.resize(size);

				auto cpuHandle = cpuHandleStart;
				auto gpuHandle = gpuHandleStart;

				for (unsigned int i = 0; i < size; i++)
				{
					m_cpuHandle[i] = cpuHandle;
					m_gpuHandle[i] = gpuHandle;

					cpuHandle.ptr += inc;
					gpuHandle.ptr += inc;
				}

				return;
			}







		}
	}
}