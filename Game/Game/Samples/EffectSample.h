#pragma once
namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CSimpleMover;
	}
}
namespace nsAWA
{
	namespace nsSamples
	{
		class CEffectSample : public IGameObject
		{
		private:
			enum class EnEffectType
			{
				enLaser,
				enSquare,
				enDistortion,
				enTriggerLaser,
				enCullingSprite,
				enNum
			};
			static const wchar_t* const 
				m_kEffectFilePathArray[static_cast<int>(EnEffectType::enNum)];

		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CEffectSample() = default;
			~CEffectSample() = default;

		private:
			CEffectPlayer* m_effectArray[static_cast<int>(EnEffectType::enNum)] = {};
			int m_currentIdx = 0;
			float m_timer = 0.0f;
			bool m_isAutoPlay = false;
			nsYMEngine::nsDebugSystem::CSimpleMover* m_simpleMover = nullptr;

		};

	}
}