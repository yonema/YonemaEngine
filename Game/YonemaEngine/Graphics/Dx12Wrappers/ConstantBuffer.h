#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{

			class CConstantBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CConstantBuffer() = default;
				~CConstantBuffer();

				/**
				 * @brief この関数で定数バッファを作成した後、
				 * CreateConstantBufferView()関数で、定数バッファビューを作成してください。
				 * 値を変更する必要がないなら、Unmap()関数で、アンマップしてください。
				 * @param size データのサイズ
				 * @param objectName デバック用のオブジェクトネーム
				 * @param numCBVs 定数バッファビューの数
				 * @param srcData データ
				 * @return 定数バッファを作成できたか？
				*/
				bool Init(
					unsigned int size,
					const wchar_t* objectName = nullptr,
					unsigned int numCBVs = 1,
					const void* srcData = nullptr
				);
				void Release();

				void CreateConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHeapHandle);

				/**
				 * @brief マップされたデータにコピーする。
				 * マップ、アンマップはこの関数内では行われません。
				 * @param data コピー元のデータ
				*/
				void CopyToMappedConstantBuffer(const void* data, size_t dataSize);

				constexpr auto GetMappedConstantBuffer() noexcept
				{
					return m_mappedConstantBuffer;
				}

				inline auto GetSizeInByte() noexcept
				{
					return static_cast<unsigned int>(m_constantBuffer->GetDesc().Width) / m_numCBVs;
				}

				inline void Unmap()
				{
					CD3DX12_RANGE readRange(0, 0);
					m_constantBuffer->Unmap(0, nullptr);
					return;
				}

				void SetName(const wchar_t* objectName);

			private:
				void Terminate();

			private:
				ID3D12Resource* m_constantBuffer = nullptr;
				void* m_mappedConstantBuffer = nullptr;
				unsigned int m_numIncrements = 0;
				unsigned int m_numCBVs = 0;
			};

		}
	}
}