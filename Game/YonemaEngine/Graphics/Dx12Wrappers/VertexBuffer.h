#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CVertexBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CVertexBuffer() = default;
				~CVertexBuffer();

				/**
				 * @brief 頂点バッファを初期化
				 * @param size 全体のサイズ
				 * @param stride 1頂点のサイズ
				 * @param srcData コピー元のデータ
				 * @param objectName オブジェクトの名前
				 * @return 初期化成功か？
				*/
				bool Init(
					unsigned int size,
					unsigned int stride,
					const void* srcData,
					const wchar_t* objectName = nullptr
				);

				void Release();

				void Copy(const void* srcData);

				constexpr const auto Get() const noexcept
				{
					return m_vertexBuffer;
				}

				constexpr const auto& GetView() const noexcept
				{
					return m_vertexBufferView;
				}

				constexpr auto GetSizeInBytes() const noexcept
				{
					return m_vertexBufferView.SizeInBytes;
				}

				constexpr auto GetStrideInByte() const noexcept
				{
					return m_vertexBufferView.StrideInBytes;
				}

				constexpr unsigned int GetNumVertices() const noexcept
				{
					return GetSizeInBytes() / GetStrideInByte();
				}

				void SetName(const wchar_t* objectName);


			private:

				void Terminate();

			private:
				ID3D12Resource* m_vertexBuffer = nullptr;
				D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
			};

		}
	}
}