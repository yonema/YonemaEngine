#include "SkinGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			const wchar_t* const CSkinGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Models/SkinModelVertexShader.hlsl";
			const char* const CSkinGenericRenderer::m_kVsEntryFuncName = "VSMain";
			const wchar_t* const CSkinGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Models/BasicModelPixelShader.hlsl";
			const char* const CSkinGenericRenderer::m_kPsEntryFuncName = "PSMain";


			bool CSkinGenericRenderer::CreateShader(
				nsDx12Wrappers::CBlob* pVsBlob,
				nsDx12Wrappers::CBlob* pPsBlob
			) const noexcept
			{
				return LoadShader(
					m_kVsFilePath,
					m_kVsEntryFuncName,
					pVsBlob,
					m_kPsFilePath,
					m_kPsEntryFuncName,
					pPsBlob
				);
			}
		}
	}
}