#include "SimplePostEffectGenericRenderer.h"
#include "GraphicsEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		const wchar_t* const CSimplePostEffectGenericRenderer::m_kVsFilePath =
			L"Assets/Shaders/PeraPolygonVertexShader.hlsl";
		const char* const CSimplePostEffectGenericRenderer::m_kVsEntryFuncName = "VSMain";
		const wchar_t* const CSimplePostEffectGenericRenderer::m_kPsFilePath =
			L"Assets/Shaders/PeraPolygonPixelShader.hlsl";
		const char* const CSimplePostEffectGenericRenderer::m_kPsEntryFuncName = "PSMain";

		bool CSimplePostEffectGenericRenderer::CreateShader(
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