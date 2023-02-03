#include "ShadowMapInstancingGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			const wchar_t* const CShadowMapInstancingGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapVertexShader.hlsl";
			const char* const CShadowMapInstancingGenericRenderer::m_kVsEntryFuncName = "VSMainForInstancing";
			const wchar_t* const CShadowMapInstancingGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapPixelShader.hlsl";
			const char* const CShadowMapInstancingGenericRenderer::m_kPsEntryFuncName = "PSMain";


			bool CShadowMapInstancingGenericRenderer::CreateShader(
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