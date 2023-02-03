#include "Shader.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDx12Wrappers
		{
			const char* CShader::m_kVertexShaderModelName = "vs_5_0";
			const char* CShader::m_kPixelShaderModelName= "ps_5_0";

			bool CShader::LoadAsVertexShader(
				const wchar_t* filePath, const char* entryFuncName, CBlob* pShaderBlob)
			{
				return Load(filePath, entryFuncName, pShaderBlob, m_kVertexShaderModelName);
			}

			bool CShader::LoadAsPixelShader(
				const wchar_t* filePath, const char* entryFuncName, CBlob* pShaderBlob)
			{
				return Load(filePath, entryFuncName, pShaderBlob, m_kPixelShaderModelName);
			}

			bool CShader::Load(
				const wchar_t* filePath,
				const char* entryFuncName,
				CBlob* pShaderBlob,
				const char* shaderModelName
			)
			{
				CBlob errorBlob;
				UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
				// Enable better shader debugging with the graphics debugging tools.
				// デバック用および最適化なし
				compileFlags |= 
					D3DCOMPILE_DEBUG | 
					D3DCOMPILE_SKIP_OPTIMIZATION | 
					D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
				compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3
#endif
				auto result = D3DCompileFromFile(
					filePath,
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,	// インクルード可能にしておく
					entryFuncName,
					shaderModelName,
					compileFlags,	
					0,
					pShaderBlob->GetPp(),
					errorBlob.GetPp()
				);

				if (CheckErrorBlob(result, &errorBlob) != true)
				{
					return false;
				}

				return true;
			}


		}
	}
}