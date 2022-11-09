#include "IndexBuffer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CIndexBuffer::m_kNamePrefix = L"IndexBuffer: ";

			CIndexBuffer::~CIndexBuffer()
			{
				Terminate();
				return;
			}

			void CIndexBuffer::Terminate()
			{
				Release();
				return;
			}

			void CIndexBuffer::Release()
			{
				if (m_indexBuffer)
				{
					m_indexBuffer->Release();
					m_indexBuffer = nullptr;
				}
				return;
			}

			bool CIndexBuffer::Init(
				unsigned int size,
				const void* srcData,
				const wchar_t* objectName
			)
			{
				Release();

				static auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

				// ○インデックスバッファの生成
				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_indexBuffer)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"インデックスバッファの生成に失敗しました。");
					return false;
				}

				// 〇インデックスバッファビューの作成
				m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
				// unsigned int でインデックス配列を使用しているため、DXGI_FORMAT_R16_UINTを使用。
				m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
				m_indexBufferView.SizeInBytes = size;

				// 〇インデックス情報のコピー（マップ）
				Copy(srcData);

				return true;
			}

			void CIndexBuffer::Copy(const void* srcData)
			{
				uint8_t* mappedData;
				{
					CD3DX12_RANGE readRange(0, 0);
					m_indexBuffer->Map(0, &readRange, (void**)&mappedData);
				}

				memcpy(mappedData, srcData, m_indexBufferView.SizeInBytes);

				{
					CD3DX12_RANGE readRange(0, 0);
					m_indexBuffer->Unmap(0, nullptr);
				}

				return;
			}

			void CIndexBuffer::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_indexBuffer == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_indexBuffer->SetName(wstr.c_str());
#endif
				return;

			}


		}
	}
}