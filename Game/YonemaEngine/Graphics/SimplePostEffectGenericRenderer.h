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

		public:
			constexpr CSimplePostEffectGenericRenderer() = default;
			~CSimplePostEffectGenericRenderer() = default;

		private:
			void OverrideShader(
				const wchar_t** pVsFilePath,
				const char** pVsEntryFuncName,
				const wchar_t** pPsFilePath,
				const char** pPsEntryFuncName
			) override final;

		private:

		};
	}
}
