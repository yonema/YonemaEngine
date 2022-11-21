#include "FontRenderer.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			CFontRenderer::~CFontRenderer()
			{
				Terminate();
				return;
			}


			void CFontRenderer::Init(
				const SFontParameter& fontParam, EnFontType fontType)
			{
				SetFontParameter(fontParam);
				m_fontType = fontType;
				CFontEngine::GetInstance()->RegisterFontRenderer(*this, m_fontType);
				return;
			}

			void CFontRenderer::Terminate()
			{
				CFontEngine::GetInstance()->UnregisterFontRenderer(*this, m_fontType);
				return;
			}

		}
	}
}