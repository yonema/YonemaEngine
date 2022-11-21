#pragma once
#include "../../Game/FontTypeList.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			class CSpriteFontWrapper;
			class CFontRenderer;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			class CFontEngine : private nsUtils::SNoncopyable
			{
			private:
				using FontRendererList = std::list<const CFontRenderer*>;

			private:
				CFontEngine();
				~CFontEngine();

			public:

				static CFontEngine* CreateInstance();
				static void DeleteInstance();
				static CFontEngine* GetInstance();

				void Release();

				void ExecuteDraw(nsDx12Wrappers::CCommandList* commandList) noexcept;

				inline void RegisterFontRenderer(
					const CFontRenderer& fontRenderer,EnFontType fontType) noexcept
				{
					m_fontRendererListByFontType[static_cast<unsigned int>(fontType)].
						emplace_back(&fontRenderer);
				}

				void UnregisterFontRenderer(
					const CFontRenderer& fontRenderer, EnFontType fontType) noexcept;

			private:
				bool Init();

				void Terminate();


			private:
				static CFontEngine* m_instance;
				std::vector<CSpriteFontWrapper*> m_spriteFonts = {};
				std::vector<FontRendererList> m_fontRendererListByFontType = {};
			};

		}
	}
}