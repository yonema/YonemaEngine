#include "GenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			IGenericRenderer::~IGenericRenderer()
			{
				Terminate();
				return;
			}

			void IGenericRenderer::Terminate()
			{
				Release();
				return;
			}


			void IGenericRenderer::Release()
			{
				m_pipelineState.Release();
				m_rootSignature.Release();
				return;
			}

			bool IGenericRenderer::LoadShader(
				const wchar_t* vsFilePath,
				const char* vsEntryFuncName,
				nsDx12Wrappers::CBlob* pVsBlob,
				const wchar_t* psFilePath,
				const char* psEntryFuncName,
				nsDx12Wrappers::CBlob* pPsBlob
			)
			{
				nsDx12Wrappers::CShader shader;

				if (shader.LoadAsVertexShader(vsFilePath, vsEntryFuncName, pVsBlob) != true)
				{
					return false;
				}
				if (shader.LoadAsPixelShader(psFilePath, psEntryFuncName, pPsBlob) != true)
				{
					return false;
				}

				return true;
			}
		}
	}
}