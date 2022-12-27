#include "SoundPlayer.h"
#include <xaudio2.h>
#include "SoundSource.h"

namespace nsYMEngine
{
	namespace nsSound
	{
		void CSoundPlayer::OnDestroy()
		{
			Terminate();

			return;
		}

		void CSoundPlayer::Update(float deltaTime)
		{
			if (m_soundSourceList.empty())
			{
				return;
			}

			if (m_isLoop)
			{
				UpdateLoopPlay();
			}
			else
			{
				UpdateOneShotPlay();
			}

			return;
		}

		void CSoundPlayer::UpdateOneShotPlay()
		{
			m_soundSourceList.remove_if(
				[&](CSoundSource* ss)
				{
					if (ss->IsEnd())
					{
						DeleteGO(ss);
						return true;
					}
					return false;
				}
			);

			return;
		}

		void CSoundPlayer::UpdateLoopPlay()
		{
			if (m_soundSourceList.front()->IsEnd() != true)
			{
				return;
			}

			LoopPlay();

			return;
		}


		void CSoundPlayer::Terminate()
		{
			Release();
			return;
		}

		void CSoundPlayer::Release()
		{
			if (m_soundSourceList.empty() != true)
			{
				for (auto& ss : m_soundSourceList)
				{
					DeleteGO(ss);
				}

				m_soundSourceList.clear();
			}

			return;
		}


		void CSoundPlayer::Init(const char* filePath, EnSubmixType submixType, bool isLoop)
		{
			Release();

			m_filePath = filePath;
			m_submixType = submixType;
			m_isLoop = isLoop;

			auto* soundSource = NewGO<CSoundSource>();
			bool res = soundSource->Init(m_filePath, m_submixType);

			if (res != true)
			{
				m_filePath = nullptr;
				DeleteGO(soundSource);
				return;
			}

			if (m_isLoop)
			{
				m_soundSourceList.emplace_back(soundSource);
			}
			else
			{
				DeleteGO(soundSource);
			}

			return;
		}

		void CSoundPlayer::Init(const char* filePath, bool isLoop, EnSubmixType submixType)
		{
			Init(filePath, submixType, isLoop);

			return;
		}


		void CSoundPlayer::Play()
		{
			if (IsValid() != true)
			{
				return;
			}

			if (m_isLoop)
			{
				LoopPlay();
			}
			else
			{
				OneShotPlay();
			}

			return;
		}

		void CSoundPlayer::OneShotPlay()
		{
			auto* soundSource = NewGO<CSoundSource>();
			soundSource->Init(m_filePath, m_submixType);
			soundSource->SubmitAndPlay();
			soundSource->SetVolume(m_volume);
			m_soundSourceList.emplace_back(soundSource);

			return;
		}

		void CSoundPlayer::LoopPlay()
		{
			auto& soundSource = m_soundSourceList.front();
			soundSource->SetVolume(m_volume);
			if (soundSource->BuffersQueued() > 0)
			{
				soundSource->Play();
			}
			else
			{
				soundSource->SubmitAndPlay();
			}

			return;
		}

		void CSoundPlayer::Stop()
		{
			if (m_soundSourceList.empty())
			{
				return;
			}

			m_soundSourceList.back()->Stop();

			return;
		}

		void CSoundPlayer::Pause()
		{
			if (m_soundSourceList.empty())
			{
				return;
			}

			m_soundSourceList.back()->Pause();

			return;
		}

		bool CSoundPlayer::IsPlaying()
		{
			if (m_soundSourceList.empty())
			{
				return false;
			}

			return m_soundSourceList.back()->IsPlaying();
		}

		void CSoundPlayer::SetVolume(float volume)
		{
			m_volume = volume;
			if (m_soundSourceList.empty())
			{
				return;
			}

			if (m_isLoop)
			{
				m_soundSourceList.back()->SetVolume(m_volume);
			}

			return;
		}





	}
}