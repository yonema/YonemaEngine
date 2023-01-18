#include "InstancingGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			const wchar_t* const CInstancingGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Models/BasicModelVertexShader.hlsl";
			const char* const CInstancingGenericRenderer::m_kVsEntryFuncName = "VSMainForInstancing";
			const wchar_t* const CInstancingGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Models/BasicModelPixelShader.hlsl";
			const char* const CInstancingGenericRenderer::m_kPsEntryFuncName = "PSMain";


			bool CInstancingGenericRenderer::CreateShader(
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