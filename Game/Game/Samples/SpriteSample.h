#pragma once
namespace nsAWA
{
	namespace nsSamples
	{
		class CSpriteSample : public IGameObject
		{
		private:
			static const unsigned int m_kNumSprites = 2;
			static const char* m_kSpriteFilePath[m_kNumSprites];

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSpriteSample() = default;
			~CSpriteSample() = default;

		private:
			CSpriteRenderer* m_spriteRenderer[m_kNumSprites] = {};
		};
	}
}