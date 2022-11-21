#include "FontEngine.h"
#include "SpriteFontWrapper.h"
#include "FontRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			CFontEngine* CFontEngine::m_instance = nullptr;

			CFontEngine* CFontEngine::CreateInstance()
			{
				if (m_instance == nullptr)
				{
					m_instance = new CFontEngine();
				}
				return m_instance;
			}
			void CFontEngine::DeleteInstance()
			{
				if (m_instance)
				{
					delete m_instance;
				}
				m_instance = nullptr;
			}
			CFontEngine* CFontEngine::GetInstance()
			{
				return m_instance;
			}


			CFontEngine::CFontEngine()
			{

				if (Init() != true)
				{
					nsGameWindow::MessageBoxError(L"FontEngineの初期化に失敗しました。");
					assert(-1);
				}

				return;
			}


			CFontEngine::~CFontEngine()
			{
				Terminate();
				return;
			}

			void CFontEngine::Terminate()
			{
				Release();
				return;
			}

			void CFontEngine::Release()
			{
				for (auto& spriteFont : m_spriteFonts)
				{
					if (spriteFont)
					{
						spriteFont->Release();
					}
				}
				return;
			}



			bool CFontEngine::Init()
			{
				constexpr unsigned int numFontType = 
					static_cast<unsigned int>(EnFontType::enNum);
				m_spriteFonts.reserve(numFontType);

				for (unsigned int fontTypeIdx = 0; fontTypeIdx < numFontType; fontTypeIdx++)
				{
					auto* spFontTmp = new CSpriteFontWrapper();
					auto res = spFontTmp->Init(g_kSpriteFontFilePaths[fontTypeIdx]);

					if (res != true)
					{
						std::wstring str(g_kSpriteFontFilePaths[fontTypeIdx]);
						return false;
					}

					m_spriteFonts.emplace_back(spFontTmp);
				}

				m_fontRendererListByFontType.resize(numFontType);

				return true;
			}

			void CFontEngine::ExecuteDraw(nsDx12Wrappers::CCommandList* commandList) noexcept
			{
				constexpr unsigned int numFontType =
					static_cast<unsigned int>(EnFontType::enNum);

				// フォントの種類ごとに描画する。
				for (unsigned int fontTypeIdx = 0; fontTypeIdx < numFontType; fontTypeIdx++)
				{
					m_spriteFonts[fontTypeIdx]->BeginDraw(commandList);

					// フォントに対応するフォントレンダラーを全て描画する。
					for (const auto& fontRenderer : m_fontRendererListByFontType[fontTypeIdx])
					{
						m_spriteFonts[fontTypeIdx]->Draw(fontRenderer->GetFontParameter());
					}

					m_spriteFonts[fontTypeIdx]->EndDraw();
				}
			}

			void CFontEngine::UnregisterFontRenderer(
				const CFontRenderer& fontRenderer, EnFontType fontType) noexcept
			{
				m_fontRendererListByFontType[static_cast<unsigned int>(fontType)].remove_if(
					[&](const CFontRenderer* fontR)
					{
						if (fontR == &fontRenderer)
						{
							return true;
						}
						return false;
					}
				);

				return;
			}


		}
	}
}