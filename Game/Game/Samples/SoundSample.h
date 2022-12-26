#pragma once
#include "../../YonemaEngine/Sound/WaveFile.h"
namespace nsAWA
{
	namespace nsSamples
	{
		class CSoundSample : public IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSoundSample() = default;
			~CSoundSample() = default;

		private:
			CSoundPlayer* m_se = nullptr;
			CSoundPlayer* m_bgm = nullptr;
		};

	}
}