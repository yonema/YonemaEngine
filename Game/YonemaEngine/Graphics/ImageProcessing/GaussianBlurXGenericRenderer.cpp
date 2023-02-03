#include "GaussianBlurXGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsImageProcessing
		{
			const wchar_t* const CGaussianBlurXGenericRenderer::m_kVsFilePath =
				L"Assets/Shaders/ImageProcessing/GaussianBlurVertexShader.hlsl";
			const char* const CGaussianBlurXGenericRenderer::m_kVsEntryFuncName = "VSXBlur";
			const wchar_t* const CGaussianBlurXGenericRenderer::m_kPsFilePath =
				L"Assets/Shaders/ImageProcessing/GaussianBlurPixelShader.hlsl";
			const char* const CGaussianBlurXGenericRenderer::m_kPsEntryFuncName = "PSBlur";

			bool CGaussianBlurXGenericRenderer::CreateShader(
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