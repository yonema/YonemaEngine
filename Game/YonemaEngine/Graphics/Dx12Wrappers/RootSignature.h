#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CRootSignature : private nsUtils::SNoncopyable
			{
			private:
				static const wchar_t* const m_kNamePrefix;
			public:
				constexpr CRootSignature() = default;
				~CRootSignature();

				bool Init(
					unsigned int numParameters,
					const D3D12_ROOT_PARAMETER1* _pParameters,
					UINT numStaticSamplers = 0,
					const D3D12_STATIC_SAMPLER_DESC* _pStaticSamplers = nullptr,
					D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE,
					const wchar_t* objectName = nullptr
					);

				void Release();

				constexpr auto Get() noexcept
				{
					return m_rootSignature;
				}

				void SetName(const wchar_t* objectName);


			private:
				void Terminate();

			private:
				ID3D12RootSignature* m_rootSignature = nullptr;

			};
		}
	}
}