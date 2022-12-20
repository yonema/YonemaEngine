#include "DisplayFPS.h"


namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		bool CDisplayFPS::Start()
		{
			m_fontRenderer = NewGO<nsGraphics::nsFonts::CFontRenderer>("DisplayFPS");

			nsGraphics::nsFonts::CFontRenderer::SFontParameter fontParam(
				L"FPS = ",
				nsMath::CVector2::Zero(),
				nsMath::CVector4::White(),
				0.0f,
				0.5f,
				nsMath::CVector2::Zero(),
				EnAnchors::enTopLeft
			);
			m_fontRenderer->Init(fontParam);
			return true;
		}

		void CDisplayFPS::Update(float deltaTime)
		{
			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(m_kLengthDispText);
			wchar_t wText[m_kLengthDispText] = {};
			StringCbPrintf(wText, dispTextSize, L"FPS = %3.4f", 1.0f / deltaTime);
			m_fontRenderer->SetText(wText);
		}

		void CDisplayFPS::OnDestroy()
		{
			DeleteGO(m_fontRenderer);
		}

	}
}