#include "GaussianBlurYGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			const wchar_t* const CGaussianBlurYGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/ImageProcessing/GaussianBlurVertexShader.hlsl";
			const char* const CGaussianBlurYGenericRenderer::m_kVsEntryFuncName = "VSYBlur";
			const wchar_t* const CGaussianBlurYGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/ImageProcessing/GaussianBlurPixelShader.hlsl";
			const char* const CGaussianBlurYGenericRenderer::m_kPsEntryFuncName = "PSBlur";

			bool CGaussianBlurYGenericRenderer::CreateShader(
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