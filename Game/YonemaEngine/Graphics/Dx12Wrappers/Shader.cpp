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
				Load(filePath, entryFuncName, pShaderBlob, m_kVertexShaderModelName);
				return true;
			}

			bool CShader::LoadAsPixelShader(
				const wchar_t* filePath, const char* entryFuncName, CBlob* pShaderBlob)
			{
				Load(filePath, entryFuncName, pShaderBlob, m_kPixelShaderModelName);
				return true;
			}

			bool CShader::Load(
				const wchar_t* filePath,
				const char* entryFuncName,
				CBlob* pShaderBlob,
				const char* shaderModelName
			)
			{
				CBlob errorBlob;

#ifdef _DEBUG
				// Enable better shader debugging with the graphics debugging tools.
				// �f�o�b�N�p����эœK���Ȃ�
				UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
				UINT compileFlags = 0;
#endif

				auto result = D3DCompileFromFile(
					filePath,
					nullptr,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,	// �C���N���[�h�\�ɂ��Ă���
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