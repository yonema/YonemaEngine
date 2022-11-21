#pragma once
namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}
namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine;
		class CFontRenderer
		{
			struct SFontParameter;
		};
	}
}


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			class CSpriteFontWrapper : private nsUtils::SNoncopyable
			{
			public:
				constexpr CSpriteFontWrapper() = default;
				~CSpriteFontWrapper();

				bool Init(const wchar_t* filePath);

				void Release();

				void BeginDraw(nsDx12Wrappers::CCommandList* commandList);

				void Draw(const CFontRenderer::SFontParameter& fontParam) const;

				void EndDraw();

			private:
				void Terminate();

				void CalcPositionFromAnchar(
					const CFontRenderer::SFontParameter& fontParam, nsMath::CVector2* pPos) const;

				void CalcPivot(
					const CFontRenderer::SFontParameter& fontParam, nsMath::CVector2* pPivot) const;

			private:
				static nsMath::CVector2 m_frameBufferHalfSize;
				DirectX::SpriteBatch* m_spriteBatch = nullptr;
				DirectX::SpriteFont* m_spriteFont = nullptr;
				nsDx12Wrappers::CDescriptorHeap m_srvDescHeap;
			};

		}
	}
}