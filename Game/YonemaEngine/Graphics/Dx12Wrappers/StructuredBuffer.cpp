#include "StructuredBuffer.h"
#include "../GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const wchar_t* const CStructuredBuffer::m_kNamePrefix = L"StructuredBuffer: ";

			CStructuredBuffer::~CStructuredBuffer()
			{
				Terminate();

				return;
			}

			void CStructuredBuffer::Terminate()
			{
				Release();

				return;
			}

			void CStructuredBuffer::Release()
			{
				if (m_structuredBuffer == nullptr)
				{
					return;
				}

				{
					// intend to read from this resource on the CPU.
					CD3DX12_RANGE readRange(0, 0);
					m_structuredBuffer->Unmap(0, &readRange);
				}

				m_structuredBuffer->Release();
				m_structuredBuffer = nullptr;

				return;
			}

			bool CStructuredBuffer::Init(
				unsigned int sizeOfElement,
				unsigned int numElements,
				void* srcData,
				const wchar_t* objectName
			)
			{
				Release();

				m_sizeOfElement = sizeOfElement;
				m_numElements = numElements;

				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				const unsigned int size = m_sizeOfElement * m_numElements;
				D3D12_RESOURCE_DESC resDesc = 
					CD3DX12_RESOURCE_DESC::Buffer(size);

				auto result = device->CreateCommittedResource(
					&heapProp,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_structuredBuffer)
				);

				if (FAILED(result))
				{
					nsGameWindow::MessageBoxError(L"ストラクチャードバッファの生成に失敗しました。");
					return false;
				}

				//構造化バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする。
				//マップ、アンマップのオーバーヘッドを軽減するためにはこのインスタンスが生きている間は行わない。
				{
					// intend to read from this resource on the CPU.
					CD3DX12_RANGE readRange(0, 0);
					m_structuredBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedStructuredBuffer));
				}

				if (srcData != nullptr)
				{
					memcpy(m_mappedStructuredBuffer, srcData, size);
				}
				// ソースデータを設定してないと、設定するまでの間、描画処理は行われるけど、
				// 画面からは見えない状態になってしまう。
				// でも、初期座標設定する前ってことだから、全部が原点に描画されるより、
				// むしろ都合がいいかも。
				//else
				//{
				//	nsMath::CMatrix* tmpMat = new nsMath::CMatrix[size];
				//	for (unsigned int i = 0; i < size; i++)
				//	{
				//		tmpMat[i] = nsMath::CMatrix::Identity();
				//	}
				//	memcpy(m_mappedStructuredBuffer, tmpMat, size);
				//	delete[] tmpMat;
				//}

				if (objectName)
				{
					SetName(objectName);
				}

				return true;
			}

			void CStructuredBuffer::RegistShaderResourceView(
				D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle)
			{
				auto device = CGraphicsEngine::GetInstance()->GetDevice();

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_UNKNOWN;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Buffer.FirstElement = 0;
				srvDesc.Buffer.StructureByteStride = m_sizeOfElement;
				srvDesc.Buffer.NumElements = static_cast<UINT>(m_numElements);
				srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				device->CreateShaderResourceView(
					m_structuredBuffer,
					&srvDesc,
					descriptorHandle
				);

				return;
			}

			void CStructuredBuffer::CopyToMappedStructuredBuffer(const void* data)
			{
				memcpy(m_mappedStructuredBuffer, data, m_numElements * m_sizeOfElement);

				return;
			}

			void CStructuredBuffer::CopyToMappedStructuredBuffer(const void* data, size_t size)
			{
				memcpy(m_mappedStructuredBuffer, data, size);

				return;
			}


			void CStructuredBuffer::SetName(const wchar_t* objectName)
			{
#ifdef _DEBUG
				if (m_structuredBuffer == nullptr || objectName == nullptr)
				{
					return;
				}
				std::wstring wstr(m_kNamePrefix);
				wstr += objectName;
				m_structuredBuffer->SetName(wstr.c_str());
#endif
				return;
			}

		}
	}
}