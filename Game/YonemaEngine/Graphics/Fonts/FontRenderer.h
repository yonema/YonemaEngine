#pragma once
#include "FontEngine.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			class CFontRenderer : public nsGameObject::IGameObject
			{
			public:

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SFontParameter
				{
					constexpr SFontParameter() = default;
					constexpr SFontParameter(
						const wchar_t* text,
						const nsMath::CVector2& position = nsMath::CVector2::Zero(),
						const nsMath::CVector4& color = nsMath::CVector4::White(),
						float rotation = 0.0f,
						float scale = 1.0f,
						nsMath::CVector2 pivot = nsMath::CVector2::Center(),
						EnAnchors anchor = EnAnchors::enMiddleCenter
					) noexcept
						:text(text), position(position), color(color),
						rotation(rotation), scale(scale), pivot(pivot), anchor(anchor)
					{};

					~SFontParameter() = default;

					const wchar_t* text = nullptr;
					nsMath::CVector2 position = nsMath::CVector2::Zero();
					nsMath::CVector4 color = nsMath::CVector4::White();
					float rotation = 0.0f;
					float scale = 1.0f;
					nsMath::CVector2 pivot = nsMath::CVector2::Center();
					EnAnchors anchor = EnAnchors::enMiddleCenter;
				};

			public:
				bool Start() override final
				{
					return false;
				}

			public:
				constexpr CFontRenderer() = default;
				~CFontRenderer();

				/**
				 * @brief フォントレンダラーを初期化します。
				 * フォントタイプはこの関数でしか設定できません。
				 * 途中で変更もできません。
				 * @param fontParam フォントのパラメータ
				 * @param fontType フォントタイプ
				*/
				void Init(const SFontParameter& fontParam,
					EnFontType fontType = static_cast<EnFontType>(0));

				constexpr const auto& GetFontParameter() const noexcept
				{
					return m_fontParameter;
				}
				inline void SetFontParameter(const SFontParameter& fontParam) noexcept
				{
					m_fontParameter = fontParam;
				}

				constexpr const wchar_t* const GetText() const noexcept
				{
					return m_fontParameter.text;
				}
				/**
				 * @brief テキストには、ローカル変数を渡さないでください。
				 * メンバ変数やグローバル変数などか、文字列を直接渡してください。
				 * @param text テキスト(ローカル変数はダメ)
				*/
				constexpr void SetText(const wchar_t* text) noexcept
				{
					m_fontParameter.text = text;
				}

				constexpr const nsMath::CVector2& GetPosition() const noexcept
				{
					return m_fontParameter.position;
				}
				inline void SetPosition(const nsMath::CVector2& position) noexcept
				{
					m_fontParameter.position = position;
				}

				constexpr const nsMath::CVector4& GetColor() const noexcept
				{
					return m_fontParameter.color;
				}
				inline void SetColor(const nsMath::CVector4& color) noexcept
				{
					m_fontParameter.color = color;
				}

				constexpr float GetRotation() const noexcept
				{
					return m_fontParameter.rotation;
				}
				constexpr void SetRotation(float rotation) noexcept
				{
					m_fontParameter.rotation = rotation;
				}

				constexpr float GetScale() const noexcept
				{
					return m_fontParameter.scale;
				}
				constexpr void SetScale(float scale) noexcept
				{
					m_fontParameter.scale = scale;
				}

				constexpr const nsMath::CVector2& GetPivot() const noexcept
				{
					return m_fontParameter.pivot;
				}
				inline void SetPivot(const nsMath::CVector2& pivot) noexcept
				{
					m_fontParameter.pivot = pivot;
				}

				constexpr EnAnchors GetAnchor() const noexcept
				{
					return m_fontParameter.anchor;
				}
				constexpr void SetAnchor(EnAnchors anchor) noexcept
				{
					m_fontParameter.anchor = anchor;
				}

			private:

				void Terminate();


			private:
				SFontParameter m_fontParameter = {};
				EnFontType m_fontType = static_cast<EnFontType>(0);
			};

		}
	}
}