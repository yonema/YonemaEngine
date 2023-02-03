#include "ShadowMapSkinGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsShadow
		{
			const wchar_t* const CShadowMapSkinGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapVertexShader.hlsl";
			const char* const CShadowMapSkinGenericRenderer::m_kVsEntryFuncName = "VSMainForSkinning";
			const wchar_t* const CShadowMapSkinGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/Shadow/ShadowMapPixelShader.hlsl";
			const char* const CShadowMapSkinGenericRenderer::m_kPsEntryFuncName = "PSMain";


			bool CShadowMapSkinGenericRenderer::CreateShader(
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