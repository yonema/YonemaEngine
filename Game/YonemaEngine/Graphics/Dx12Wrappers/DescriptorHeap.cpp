#include "DescriptorHeap.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CDescriptorHeap::m_kNamePrefix = L"DescriptorHeap: ";

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
				UINT numDescriptors,
				D3D12_DESCRIPTOR_HEAP_FLAGS flags,
				const wchar_t* objectName
			)
			{
				Release();

				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
				descHeapDesc.Type = type;
				descHeapDesc.NumDescriptors = numDescriptors;
				descHeapDesc.Flags = flags;
				descHeapDesc.NodeMask = 0;

				auto result = device->CreateDescriptorHeap(
					&descHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"ディスクリプタヒープの生成に失敗しました。");
					return false;
				}

				SetName(objectName);

				return true;
			}

			bool CDescriptorHeap::InitAsCbvSrvUav(
				UINT numDescriptors, const wchar_t* objectName, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptors, flags, objectName);
			}
			bool CDescriptorHeap::InitAsRTV(UINT numDescriptors, const wchar_t* objectName)
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, objectName);
			}
			bool CDescriptorHeap::InitAsDSV(const wchar_t* objectName)
			{
				return Init(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, objectName);
			}

			void CDescriptorHeap::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_descriptorHeap == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_descriptorHeap->SetName(wstr.c_str());
#endif

				return;
			}


		}
	}
}
