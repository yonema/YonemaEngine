#include "SoundSample.h"
#include "../../YonemaEngine/Sound/SoundEngine.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CSoundSample::Start()
		{
			// SE�^�C�v�̃����V���b�g�Đ��p�ŏ�����
			m_se = NewGO<CSoundPlayer>();
			m_se->Init("Assets/Sounds/Samples/tokimekiExperience_short.wav", EnSubmixType::enSE, false);

			// BGM�^�C�v�̃��[�v�����p�ŏ�����
			m_bgm = NewGO<CSoundPlayer>();
			m_bgm->Init("Assets/Sounds/Samples/tokimekiExperience_short.wav", EnSubmixType::enBGM, true);

			// BGM�^�C�v�S�̂̉��ʐݒ�
			nsYMEngine::nsSound::CSoundEngine::GetInstance()->SetSubmixVolume(
				EnSubmixType::enBGM, 3.0f);

			return true;
		}

		void CSoundSample::OnDestroy()
		{
			DeleteGO(m_bgm);
			DeleteGO(m_se);

			return;
		}

		void CSoundSample::Update(float deltaTime)
		{
			// �����V���b�g�Đ��̂r�d���Đ�
			if (Keyboard()->IsTrigger(EnKeyButton::enS))
			{
				static int a = 0;
				float volume = 1.0f;
				switch (a++)
				{
				case 1:
					volume = 5.0f;
					break;
				case 2:
					volume = 10.0f;
					break;
				default:
					break;
				}
				m_se->SetVolume(volume);
				m_se->Play();
			}

			// ���[�v�Đ��̂a�f�l���Đ�
			if (Keyboard()->IsTrigger(EnKeyButton::enB))
			{
				if (m_bgm->IsPlaying() != true)
				{
					m_bgm->Play();
				}
				else
				{
					if (Keyboard()->IsPress(EnKeyButton::enShift))
					{
						m_bgm->Pause();
					}
					else
					{
						m_bgm->Stop();
					}
				}

			}

			return;
		}
	}
}