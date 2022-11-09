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

		void CSimplePostEffectGenericRenderer::OverrideShader(
			const wchar_t** pVsFilePath,
			const char** pVsEntryFuncName,
			const wchar_t** pPsFilePath,
			const char** pPsEntryFuncName
		)
		{
			*pVsFilePath = m_kVsFilePath;
			*pVsEntryFuncName = m_kVsEntryFuncName;
			*pPsFilePath = m_kPsFilePath;
			*pPsEntryFuncName = m_kPsEntryFuncName;

			return;
		}
	}
}