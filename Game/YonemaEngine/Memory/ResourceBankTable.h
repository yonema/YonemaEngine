#pragma once
#include "ResourceBank.h"
#include "../Sound/WaveFile.h"
#include "../Graphics/Animations/AnimationClip.h"
#include "../Graphics/Dx12Wrappers/Texture.h"

namespace nsYMEngine
{
	namespace nsMemory
	{
		class CResourceBankTable : private nsUtils::SNoncopyable
		{
		private:
			constexpr CResourceBankTable() = default;
			~CResourceBankTable() = default;

		public:
			static inline CResourceBankTable* CreateInstance() noexcept
			{
				if (m_instance == nullptr)
				{
					m_instance = new CResourceBankTable;
				}

				return m_instance;
			}

			static inline CResourceBankTable* GetInstance() noexcept
			{
				return m_instance;
			}

			static void DeleteInstance() noexcept
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}

			constexpr auto& GetWaveFileBank() noexcept
			{
				return m_waveFileBank;
			}

			constexpr auto& GetAnimationClipBank() noexcept
			{
				return m_animationClipBank;
			}

			constexpr auto& GetTextureBank() noexcept
			{
				return m_textureBank;
			}


		private:
			static CResourceBankTable* m_instance;
			TRsourceBank<nsSound::CWaveFile> m_waveFileBank = {};
			TRsourceBank<nsGraphics::nsAnimations::CAnimationClip> m_animationClipBank = {};
			TRsourceBank<nsGraphics::nsDx12Wrappers::CTexture> m_textureBank = {};
		};

	}
}