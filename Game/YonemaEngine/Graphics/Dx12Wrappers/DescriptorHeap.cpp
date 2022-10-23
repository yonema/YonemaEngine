#include "DescriptorHeap.h"
#include "../GraphicsEngine.h"
#include "../../GameWindow/MessageBox.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			CDescriptorHeap::~CDescriptorHeap()
			{
				Terminate();

				return;
			}

			void CDescriptorHeap::Terminate()
			{
				Release();

				return;
			}

			void CDescriptorHeap::Release()
			{
				if (m_descriptorHeap)
				{
					m_descriptorHeap->Release();
					m_descriptorHeap = nullptr;
				}

				return;
			}

			bool CDescriptorHeap::Init(
				D3D12_DESCRIPTOR_HEAP_TYPE type,
				UINT numDescHeaps,
				D3D12_DESCRIPTOR_HEAP_FLAGS flags
			)
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
				descHeapDesc.Type = type;
				descHeapDesc.NumDescriptors = numDescHeaps;
				descHeapDesc.Flags = flags;
				descHeapDesc.NodeMask = 0;

				auto result = device->CreateDescriptorHeap(
					&descHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"ディスクリプタヒープの生成に失敗しました。");
					return false;
				}

				return true;
			}

			bool CDescriptorHeap::InitAsCbvSrvUav(
				UINT numDescHeaps, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescHeaps, flags);
			}
			bool CDescriptorHeap::InitAsRTV(UINT numDescHeaps)
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescHeaps, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
			}
			bool CDescriptorHeap::InitAsDSV()
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
			}

		}
	}
}
