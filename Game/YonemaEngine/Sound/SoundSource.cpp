#include "SoundSource.h"
#include "SoundEngine.h"
#include "WaveFile.h"
#include "../Memory/ResourceBankTable.h"
#include "../Utils/StringManipulation.h"

namespace nsYMEngine
{
	namespace nsSound
	{
		bool CSoundSource::Start()
		{

			return true;
		}

		void CSoundSource::OnDestroy()
		{
			Release();
			return;
		}

		void CSoundSource::Update(float deltaTime)
		{
			if (m_isPlaying != true || m_isEnd == true)
			{
				return;
			}


			if (BuffersQueued() <= 0)
			{
				m_isEnd = true;
				m_isPlaying = false;
			}


			return;
		}

		void CSoundSource::Release()
		{
			if (m_sourceVoice != nullptr)
			{
				m_sourceVoice->FlushSourceBuffers();
				m_sourceVoice->DestroyVoice();
				m_sourceVoice = nullptr;
			}
			return;
		}


		bool CSoundSource::Init(const char* filePath, EnSubmixType submixType)
		{
			Release();

			auto& waveFileBank = nsMemory::CResourceBankTable::GetInstance()->GetWaveFileBank();
			m_waveFalie = waveFileBank.Get(filePath);
			if (m_waveFalie == nullptr)
			{
				m_waveFalie = new CWaveFile();
				std::wstring wFilePath = nsUtils::GetWideStringFromString(filePath);
				bool res = m_waveFalie->Init(wFilePath.c_str());

				if (res != true)
				{
					std::wstring str = wFilePath;
					str.erase(str.end() - 1);
					str += L"のサウンドの読み込みに失敗しました。";
					nsGameWindow::MessageBoxError(str.c_str());
					return false;
				}

				waveFileBank.Register(filePath, m_waveFalie);
			}

			auto* soundEngine = CSoundEngine::GetInstance();
			bool res = soundEngine->CreateSoundVoice(
				&m_sourceVoice, m_waveFalie->GetFormatex(), submixType);

			return res;
		}

		void CSoundSource::Play()
		{
			m_sourceVoice->Start(0);

			m_isPlaying = true;
			m_isEnd = false;

			return;
		}


		void CSoundSource::SubmitAndPlay()
		{
			m_sourceVoice->FlushSourceBuffers();
			m_sourceVoice->SubmitSourceBuffer(m_waveFalie->GetXAudioBuffer());

			Play();

			return;
		}

		void CSoundSource::Stop()
		{
			m_sourceVoice->Stop();
			m_sourceVoice->FlushSourceBuffers();
			m_isPlaying = false;
			return;
		}

		void CSoundSource::Pause()
		{
			m_sourceVoice->Stop();
			m_isPlaying = false;
			return;
		}

		unsigned int CSoundSource::BuffersQueued() const
		{
			XAUDIO2_VOICE_STATE state;
			m_sourceVoice->GetState(&state);
			return state.BuffersQueued;
		}

		void CSoundSource::SetVolume(float volume)
		{
			m_sourceVoice->SetVolume(volume);
			return;
		}

		float CSoundSource::GetVolume() const
		{
			float volume;
			m_sourceVoice->GetVolume(&volume);
			return volume;
		}





	}
}