#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CBlob : private nsUtils::SNoncopyable
			{
			public:
				constexpr CBlob() = default;
				~CBlob();

				void Release();

				constexpr ID3DBlob* Get() noexcept
				{
					return m_blob;
				}

				constexpr ID3DBlob** GetPp() noexcept
				{
					return &m_blob;
				}

			private:

				void Terminate();

			private:
				ID3DBlob* m_blob = nullptr;
			};

			/**
			 * @brief �G���[�p��Blob���G���[�`�F�b�N���܂��B
			 * errorBlob�͂��̊֐��̓�����Release����܂��B
			 * @param result HRESULT
			 * @param errorBlob �G���[�p��Blob
			 * @return �G���[���Ȃ���
			 * @retval true �G���[�Ȃ�
			 * @retval false �G���[����
			*/
			bool CheckErrorBlob(HRESULT result, CBlob* errorBlob);

		}
	}
}