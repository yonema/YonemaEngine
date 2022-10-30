#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			class CShader : private nsUtils::SNoncopyable
			{
			private:
				static const char* m_kVertexShaderModelName;
				static const char* m_kPixelShaderModelName;

			public:
				constexpr CShader() = default;
				~CShader() = default;

				bool LoadAsVertexShader(
					const wchar_t* filePath, const char* entryFuncName, CBlob* pShaderBlob);

				bool LoadAsPixelShader(
					const wchar_t* filePath, const char* entryFuncName, CBlob* pShaderBlob);


			private:

				bool Load(
					const wchar_t* filePath,
					const char* entryFuncName,
					CBlob* pShaderBlob,
					const char* shaderModelName
				);

			private:

			};

		}
	}
}