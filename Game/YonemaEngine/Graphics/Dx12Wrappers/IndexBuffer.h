#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CIndexBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CIndexBuffer() = default;
				~CIndexBuffer();

				bool Init(
					unsigned int size,
					const void* srcData,
					const wchar_t* objectName = nullptr
				);

				void Release();

				void Copy(const void* srcData);

				constexpr const auto Get() const noexcept
				{
					return m_indexBuffer;
				}

				constexpr const auto& GetView() const noexcept
				{
					return m_indexBufferView;
				}

				constexpr auto GetSizeInBytes() const noexcept
				{
					return m_indexBufferView.SizeInBytes;
				}

				void SetName(const wchar_t* objectName);

			private:

				void Terminate();

			private:
				ID3D12Resource* m_indexBuffer = nullptr;
				D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};

			};

		}
	}
}