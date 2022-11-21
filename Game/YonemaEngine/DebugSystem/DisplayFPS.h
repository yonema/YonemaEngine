#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			class CFontRenderer;
		}
	}
}

namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CDisplayFPS : public nsGameObject::IGameObject
		{
		private:
			static const int m_kLengthDispText = 32;
		public:
			bool Start() override final;
			void Update(float deltaTime) override final;
			void OnDestroy() override final;

		public:
			constexpr CDisplayFPS() = default;
			~CDisplayFPS() = default;


		private:
			nsGraphics::nsFonts::CFontRenderer* m_fontRenderer = nullptr;
			wchar_t m_dispText[m_kLengthDispText] = {};
		};

	}
}