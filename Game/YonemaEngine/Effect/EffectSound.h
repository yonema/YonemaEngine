#pragma once
#include <EffekseerSound/EffekseerSoundXAudio2.SoundLoader.h>
#include <EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.h>

namespace nsYMEngine
{
	namespace nsEffect
	{
		//////// CEffectSoundLoader ////////

		class CEffectSoundLoader : public ::EffekseerSound::SoundLoader
		{
		private:
			using SoundImplementedRef = ::Effekseer::RefPtr<EffekseerSound::SoundImplemented>;

		public:
			CEffectSoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface = nullptr)
				: SoundLoader(sound, fileInterface)
			{
				return;
			}
			~CEffectSoundLoader() = default;

			void Unload(::Effekseer::SoundDataRef soundData) override final;
		};



		//////// CEffectSoundPlayer ////////

		class CEffectSoundPlayer : public EffekseerSound::SoundPlayer
		{
		private:
			using SoundImplementedRef = ::Effekseer::RefPtr<EffekseerSound::SoundImplemented>;

		public:
			CEffectSoundPlayer(const SoundImplementedRef& sound)
				: SoundPlayer(sound)
			{
				return;
			};

			~CEffectSoundPlayer() = default;

		public:
			Effekseer::SoundHandle Play(
				Effekseer::SoundTag tag, const InstanceParameter& parameter) override final;

		};



	}
}