#pragma once
#include "Sprites/SpriteGenericRenderer.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CSimplePostEffectGenericRenderer : public nsSprites::CSpriteGenericRenderer
		{
		private:
			static const wchar_t* const m_kVsFilePath;
			static const char* const m_kVsEntryFuncName;
			static const wchar_t* const m_kPsFilePath;
			static const char* const m_kPsEntryFuncName;

		private:
			inline std::wstring CreateRootSignatureName() const noexcept override
			{
				return L"SimplePostEffectGenericRenderer";
			}
			bool CreateShader(
				nsDx12Wrappers::CBlob* pVsBlob,
				nsDx12Wrappers::CBlob* pPsBlob
			) const noexcept override;

			inline std::wstring CreatePipelineStateName() const noexcept override
			{
				return L"SimplePostEffectGenericRenderer";
			}

		public:
			constexpr CSimplePostEffectGenericRenderer() = default;
			~CSimplePostEffectGenericRenderer() = default;

		private:

		};
	}
}
