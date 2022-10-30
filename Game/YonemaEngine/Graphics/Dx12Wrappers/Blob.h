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
			 * @brief エラー用のBlobをエラーチェックします。
			 * errorBlobはこの関数の内部でReleaseされます。
			 * @param result HRESULT
			 * @param errorBlob エラー用のBlob
			 * @return エラーがないか
			 * @retval true エラーなし
			 * @retval false エラーあり
			*/
			bool CheckErrorBlob(HRESULT result, CBlob* errorBlob);

		}
	}
}