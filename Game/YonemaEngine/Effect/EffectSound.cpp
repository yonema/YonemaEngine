#include "EffectSound.h"
#include <../EffekseerSoundXAudio2/EffekseerSound/EffekseerSoundXAudio2.SoundVoice.h>

namespace nsYMEngine
{
	namespace nsEffect
	{
		////////// CEffectSoundLoader ////////

		//void CEffectSoundLoader::Unload(::Effekseer::SoundDataRef soundData)
		//{
		//	if (soundData != nullptr)
		//	{
		//		// stop a voice which plays this data
		//		auto& sound = GetSound();
		//		sound->StopData(soundData);
		//		EffekseerSound::SoundData* soundDataImpl = (EffekseerSound::SoundData*)soundData.Get();

		//		// なんか止まってくれないから、直接SourceVoiceを取り出して止める。
		//		auto* container = sound->GetContainer();
		//		for (int channelIdx = 0; channelIdx < 2; channelIdx++)
		//		{
		//			auto& voiceList = container[channelIdx]->GetVoiceList();
		//			for (auto voice : voiceList)
		//			{
		//				auto* sourceVoice = voice->GetSourceVoice();
		//				sourceVoice->FlushSourceBuffers();
		//				sourceVoice->DestroyVoice();
		//			}
		//			voiceList.clear();
		//		}

		//		ES_SAFE_DELETE_ARRAY(soundDataImpl->GetBufferRef().pAudioData);
		//	}

		//	return;
		//}


		CEffectSoundLoader::~CEffectSoundLoader()
		{
			Terminate();

			return;
		}

		void CEffectSoundLoader::Terminate()
		{
			// なんか止まってくれないから、直接SourceVoiceを取り出して止める。
			auto& sound = GetSound();
			auto* container = sound->GetContainer();
			for (int channelIdx = 0; channelIdx < 2; channelIdx++)
			{
				auto& voiceList = container[channelIdx]->GetVoiceList();
				for (auto voice : voiceList)
				{
					auto* sourceVoice = voice->GetSourceVoice();
					sourceVoice->FlushSourceBuffers();
					sourceVoice->DestroyVoice();
				}
				voiceList.clear();
			}

			return;
		}



		//////// CEffectSoundPlayer ////////

		Effekseer::SoundHandle CEffectSoundPlayer::Play(
			Effekseer::SoundTag tag, const InstanceParameter& parameter)
		{
			auto& sound = GetSound();
			if (sound->GetMute())
			{
				return nullptr;
			}

			if (parameter.Data != nullptr)
			{
				EffekseerSound::SoundData* soundDataImpl = (EffekseerSound::SoundData*)parameter.Data.Get();
				EffekseerSound::SoundVoice* voice = sound->GetVoice(soundDataImpl->GetChannels());
				if (voice)
				{
					auto* sourceVoice = voice->GetSourceVoice();
					auto* buffer = soundDataImpl->GetBuffer();

					EffekseerSound::SoundData* soundData = (EffekseerSound::SoundData*)parameter.Data.Get();

					sourceVoice->SetSourceSampleRate(soundData->GetSampleRate());
					sourceVoice->SetVolume(parameter.Volume);
					sourceVoice->SetFrequencyRatio(powf(2.0f, parameter.Pitch));

					// SubmitSourceBuffer()は後
					sourceVoice->SubmitSourceBuffer(soundData->GetBuffer());

					float matrix[2 * 4];
					if (parameter.Mode3D)
					{
						sound->Calculate3DSound(parameter.Position, parameter.Distance, soundData->GetChannels(), 2, matrix);
					}
					else
					{
						float rad = ((parameter.Pan + 1.0f) * 0.5f) * (3.1415926f * 0.5f);
						switch (soundData->GetChannels())
						{
						case 1:
							matrix[0] = cosf(rad);
							matrix[1] = sinf(rad);
							break;
						case 2:
							matrix[0] = matrix[3] = 1.0f;
							matrix[1] = matrix[2] = 0.0f;
							break;
						default:
							return (::Effekseer::SoundHandle)voice;
						}
					}
					sourceVoice->SetOutputMatrix(nullptr, soundData->GetChannels(), 2, matrix);
					sourceVoice->Start();

					// このPlay()関数では、SetSourceSampleRate()がSubmitSourceBuffer()より先に
					// 呼ばれてしまって、エラーが出るため、手動で再生する。

					//voice->Play(tag, parameter);

					return (::Effekseer::SoundHandle)voice;
				}
			}
			return nullptr;
		}






	}
}