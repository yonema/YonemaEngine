#include "FontSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CFontSample::Start()
		{
			constexpr int numFontRenderers = 3;
			constexpr const wchar_t* const fontText[numFontRenderers] =
			{
				L"戸山香澄",
				L"バンドリ!",
				L"キラキラドキドキ"
			};
			m_fontRenderers.resize(numFontRenderers);
			int index = 0;
			for (auto& fontRenderer : m_fontRenderers)
			{
				SFontParameter fontParam;
				fontParam.text = fontText[index];
				float rate = static_cast<float>(index) / static_cast<float>(numFontRenderers - 1);
				fontParam.position.x = nsMath::Lerpf(rate, -500.0f, 500.0f);
				fontParam.position.y = 0.0f;
				fontParam.pivot.x = nsMath::Lerpf(rate, 0.0f, 1.0f);
				fontParam.pivot.y = nsMath::Lerpf(rate, 0.0f, 1.0f);
				fontParam.anchor = EnAnchors::enMiddleCenter;
				fontRenderer = NewGO<CFontRenderer>();
				if (index == 2)
				{
					fontRenderer->Init(
						fontParam, EnFontType::enDegitalTextbook);
				}
				else
				{
					fontRenderer->Init(fontParam);
				}
				index++;
			}

			return true;
		}

		void CFontSample::OnDestroy()
		{
			for (auto& fontRenderer : m_fontRenderers)
			{
				DeleteGO(fontRenderer);
			}
			return;
		}

		void CFontSample::Update(float deltaTime)
		{
			// フォント回転
			for (auto& fontRenderer : m_fontRenderers)
			{
				if (fontRenderer)
				{
					float rotTmp = fontRenderer->GetRotation();
					rotTmp += 3.14f / 180.0f * 50.0f * deltaTime;
					fontRenderer->SetRotation(rotTmp);
				}
			}
			return;
		}
	}
}