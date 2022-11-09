#include "ConstantBuffer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CConstantBuffer::m_kNamePrefix = L"ConstantBuffer: ";

			CConstantBuffer::~CConstantBuffer()
			{
				Terminate();

				return;
			}

			void CConstantBuffer::Terminate()
			{
				Release();

				return;
			}

			void CConstantBuffer::Release()
			{
				if (m_constantBuffer)
				{
					m_constantBuffer->Release();
					m_constantBuffer = nullptr;
				}

				return;
			}


			bool CConstantBuffer::Init(
				unsigned int size,
				const wchar_t* objectName,
				unsigned int numCBVs,
				const void* srcData
			)
			{
				Release();

				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				auto tmpSize = size;
				// 無駄な領域は生じるが、バッファサイズを256アライメントにする
				size = (size + 0xff) & ~0xff;
				m_numCBVs = numCBVs;
				size *= m_numCBVs;
				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_constantBuffer)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"定数バッファの生成に失敗しました。");
					return false;
				}

				// @todo よくわからんがreadRangeを設定しておく。
				//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
				{
					// intend to read from this resource on the CPU.
					// (訳)CPUのこのリソースから読み出すことを意図している
					CD3DX12_RANGE readRange(0, 0);
					result = m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedConstantBuffer));
				}

				if (srcData)
				{
					CopyToMappedConstantBuffer(srcData, tmpSize);
				}

				if (objectName)
				{
					SetName(objectName);
				}

				return true;
			}

			void CConstantBuffer::CreateConstantBufferView(
				D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHeapHandle)
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();
				auto  bufferSize = GetSizeInByte();

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
				cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
				cbvDesc.BufferLocation += bufferSize * m_numIncrements;
				m_numIncrements++;
				cbvDesc.SizeInBytes = bufferSize;

				device->CreateConstantBufferView(&cbvDesc, cpuDescHeapHandle);

				return;
			}


			void CConstantBuffer::CopyToMappedConstantBuffer(const void* data, size_t dataSize)
			{
				//unsigned int bufferSize = static_cast<unsigned int>(m_constantBuffer->GetDesc().Width);

				memcpy(m_mappedConstantBuffer, data, dataSize);

				return;
			}


			void CConstantBuffer::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_constantBuffer == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_constantBuffer->SetName(wstr.c_str());
#endif
				return;
			}

		}
	}
}