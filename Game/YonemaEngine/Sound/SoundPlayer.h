#pragma once
#include "../../Game/SubmixType.h"
namespace nsYMEngine
{
	namespace nsSound
	{
		class CSoundSource;
	}
}

namespace nsYMEngine
{
	namespace nsSound
	{
		class CSoundPlayer : public nsGameObject::IGameObject
		{
		public:
			bool Start() override final
			{
				return true;
			}

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSoundPlayer() = default;
			~CSoundPlayer() = default;

			void Init(
				const char* filePath,
				EnSubmixType submixType = EnSubmixType::enSE,
				bool isLoop = false
			);

			void Init(
				const char* filePath,
				bool isLoop = false,
				EnSubmixType submixType = EnSubmixType::enSE
			);

			void Release();

			constexpr bool IsValid() const noexcept
			{
				return m_filePath != nullptr;
			}

			void Play();

			void Stop();

			void Pause();

			bool IsPlaying();

			void SetVolume(float volume);

			constexpr float GetVolume() const noexcept
			{
				return m_volume;
			}


		private:

			void Terminate();

			void OneShotPlay();

			void LoopPlay();

			void UpdateOneShotPlay();

			void UpdateLoopPlay();

		private:
			std::list<CSoundSource*> m_soundSourceList = {};
			const char* m_filePath = nullptr;
			EnSubmixType m_submixType = EnSubmixType::enSE;
			bool m_isPlaying = false;
			bool m_isLoop = false;
			float m_volume = 1.0f;
		};

	}
}