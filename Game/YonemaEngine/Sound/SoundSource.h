#pragma once
#include "../../Game/SubmixType.h"

namespace nsYMEngine
{
	namespace nsSound
	{
		class CWaveFile;
	}
}
struct IXAudio2SourceVoice;

namespace nsYMEngine
{
	namespace nsSound
	{
		class CSoundSource : public nsGameObject::IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CSoundSource() = default;
			~CSoundSource() = default;

			bool Init(const char* filePath, EnSubmixType submixType);

			constexpr bool IsValid() const noexcept
			{
				return m_sourceVoice != nullptr ? true : false;
			}

			constexpr bool IsEnd() const noexcept
			{
				return m_isEnd;
			}

			constexpr bool IsPlaying() const noexcept
			{
				return m_isPlaying;
			}

			void Play();

			void SubmitAndPlay();

			void Stop();

			void Pause();

			unsigned int BuffersQueued() const;

			void SetVolume(float volume);

			float GetVolume() const;

			void Release();

		private:
			IXAudio2SourceVoice* m_sourceVoice = nullptr;
			CWaveFile* m_waveFalie = nullptr;
			bool m_isPlaying = false;
			bool m_isEnd = false;
		};

	}
}