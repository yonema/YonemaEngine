#pragma once
#include <xaudio2.h>
#include "../../Game/SubmixType.h"

namespace nsYMEngine
{
	namespace nsSound
	{

		class CSoundEngine : private nsUtils::SNoncopyable
		{
		private:
			CSoundEngine();
			~CSoundEngine();

		public:
			static CSoundEngine* CreateInstance()
			{
				if (m_instance == nullptr)
				{
					m_instance = new CSoundEngine();
				}
				return m_instance;
			}

			static inline CSoundEngine* GetInstance()
			{
				return m_instance;
			}

			static void DeleteInstance()
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}

			bool CreateSoundVoice(
				IXAudio2SourceVoice** pSoundVoice,
				const WAVEFORMATEX* wfx,
				EnSubmixType submixType
			);

			inline void SetSubmixVolume(EnSubmixType submixType, float volume)
			{
				if (submixType < EnSubmixType::enNum)
				{
					m_submixVoiceArray[static_cast<int>(submixType)]->SetVolume(volume);
				}
			}

			inline float GetSubmixVolume(EnSubmixType submixType)
			{
				float volume = 1.0f;
				if (submixType < EnSubmixType::enNum)
				{
					m_submixVoiceArray[static_cast<int>(submixType)]->GetVolume(&volume);
				}
				return volume;
			}

		private:
			void Init();

			void Terminate();

			void CreateXAudio();

			void CreateMasteringVoice();

			void CreateSubmixVoice();


		private:
			static CSoundEngine* m_instance;
			IXAudio2* m_xAudio = nullptr;
			IXAudio2MasteringVoice* m_masteringVoice = nullptr;
			IXAudio2SubmixVoice* m_submixVoiceArray[static_cast<int>(EnSubmixType::enNum)];
		};
	}
}
