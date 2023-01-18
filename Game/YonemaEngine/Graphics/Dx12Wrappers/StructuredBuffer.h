#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CStructuredBuffer : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;

			public:
				constexpr CStructuredBuffer() = default;
				~CStructuredBuffer();

				void Release();

				bool Init(
					unsigned int sizeOfElement,
					unsigned int numElements,
					void* srcData = nullptr,
					const wchar_t* objectName = nullptr
				);

				void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle);

				void CopyToMappedStructuredBuffer(const void* data);

				constexpr auto* GetMappedStructuredBuffer() noexcept
				{
					return m_mappedStructuredBuffer;
				}

				void SetName(const wchar_t* objectName);


			private:

				void Terminate();

			private:
				ID3D12Resource* m_structuredBuffer = nullptr;
				void* m_mappedStructuredBuffer = nullptr;
				unsigned int m_sizeOfElement = 0;
				unsigned int m_numElements = 0;
			};

		}
	}
}