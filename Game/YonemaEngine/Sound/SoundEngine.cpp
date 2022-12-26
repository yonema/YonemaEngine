#include "SoundEngine.h"
namespace nsYMEngine
{
	namespace nsSound
	{
		CSoundEngine* CSoundEngine::m_instance = nullptr;

		CSoundEngine::CSoundEngine()
		{
			Init();

			return;
		}

		CSoundEngine::~CSoundEngine()
		{
			Terminate();

			return;
		}

		void CSoundEngine::Terminate()
		{
			for (int i = 0; i < static_cast<int>(EnSubmixType::enNum); i++)
			{
				if (m_submixVoiceArray[i])
				{
					m_submixVoiceArray[i]->DestroyVoice();
					m_submixVoiceArray[i] = nullptr;
				}
			}
			if (m_masteringVoice != 0)
			{
				m_masteringVoice->DestroyVoice();
				m_masteringVoice = nullptr;
			}
			if (m_xAudio != 0)
			{
				m_xAudio->Release();
				m_xAudio = nullptr;
			}
			return;
		}

		void CSoundEngine::Init()
		{
			CreateXAudio();

			CreateMasteringVoice();

			CreateSubmixVoice();



			return;
		}


		void CSoundEngine::CreateXAudio()
		{
			UINT32 flags = 0;
#ifdef _DEBUG
			flags |= XAUDIO2_DEBUG_ENGINE;
#endif

			auto res = XAudio2Create(&m_xAudio, flags, XAUDIO2_DEFAULT_PROCESSOR);

			if (FAILED(res))
			{
				nsGameWindow::MessageBoxError(L"XAudioの作成に失敗しました。");
				return;
			}

#ifdef _DEBUG
			XAUDIO2_DEBUG_CONFIGURATION debug = {};
			debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			debug.BreakMask = XAUDIO2_LOG_ERRORS;
			m_xAudio->SetDebugConfiguration(&debug, 0);
#endif
			return;
		}

		void CSoundEngine::CreateMasteringVoice()
		{
			auto res = m_xAudio->CreateMasteringVoice(&m_masteringVoice);

			if (FAILED(res))
			{
				nsGameWindow::MessageBoxError(L"マスタリング音声の作成に失敗しました。");
				return;
			}

			return;
		}

		void CSoundEngine::CreateSubmixVoice()
		{
			for (int i = 0; i < static_cast<int>(EnSubmixType::enNum); i++)
			{
				m_xAudio->CreateSubmixVoice(&m_submixVoiceArray[i], 1, 44100, 0, 0, 0, 0);
			}

			return;
		}


		bool CSoundEngine::CreateSoundVoice(
			IXAudio2SourceVoice** pSoundVoice,
			const WAVEFORMATEX* wfx,
			EnSubmixType submixType
		)
		{
			if (m_xAudio == nullptr || submixType >= EnSubmixType::enNum)
			{
				return false;
			}

			XAUDIO2_SEND_DESCRIPTOR sendDesc = 
			{ 0, m_submixVoiceArray[static_cast<int>(submixType)] };
			XAUDIO2_VOICE_SENDS sendList = { 1, &sendDesc };

			auto hRes = m_xAudio->CreateSourceVoice(
				pSoundVoice,
				wfx,
				0,
				XAUDIO2_DEFAULT_FREQ_RATIO,
				nullptr,
				&sendList,
				nullptr
			);
			return SUCCEEDED(hRes);
		}













	}
}