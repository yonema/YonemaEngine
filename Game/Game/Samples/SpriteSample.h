#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CSpriteSample : public IGameObject
		{
		private:
			static const char* m_kSpriteFilePath;

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSpriteSample() = default;
			~CSpriteSample() = default;

		private:
			CSpriteRenderer* m_spriteRenderer = nullptr;
		};
	}
}