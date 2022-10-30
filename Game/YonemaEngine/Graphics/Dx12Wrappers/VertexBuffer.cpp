#include "VertexBuffer.h"
#include "../GraphicsEngine.h"
#include "../../GameWindow/MessageBox.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CVertexBuffer::m_kNamePrefix = L"VertexBuffer: ";

			CVertexBuffer::~CVertexBuffer()
			{
				Terminate();
				return;
			}

			void CVertexBuffer::Terminate()
			{
				Release();
				return;
			}

			void CVertexBuffer::Release()
			{
				if (m_vertexBuffer)
				{
					m_vertexBuffer->Release();
					m_vertexBuffer = nullptr;
				}
				return;
			}

			bool CVertexBuffer::Init(
				unsigned int size,
				unsigned int stride,
				const void* srcData,
				const wchar_t* objectName
			)
			{
				Release();

				static auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

				// ○頂点バッファの生成
				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_vertexBuffer)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxWarning(L"頂点バッファの生成に失敗しました。");
					return false;
				}

				SetName(objectName);

				// 〇頂点バッファビューの作成
				m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
				m_vertexBufferView.SizeInBytes = static_cast<UINT>(size);
				m_vertexBufferView.StrideInBytes = static_cast<UINT>(stride);

				// 〇頂点情報のコピー（マップ）
				if (srcData)
				{
					Copy(srcData);
				}

				return true;
			}

			void CVertexBuffer::Copy(const void* srcData)
			{
				uint8_t* mappedData;
				{
					CD3DX12_RANGE readRange(0, 0);
					m_vertexBuffer->Map(0, &readRange, (void**)&mappedData);
				}

				memcpy(mappedData, srcData, m_vertexBufferView.SizeInBytes);

				{
					CD3DX12_RANGE readRange(0, 0);
					m_vertexBuffer->Unmap(0, nullptr);
				}				

				return;
			}

			void CVertexBuffer::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_vertexBuffer == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_vertexBuffer->SetName(wstr.c_str());
#endif
				return;

			}


		}
	}
}
