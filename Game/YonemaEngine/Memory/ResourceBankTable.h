#pragma once
#include "ResourceBank.h"
#include "../Sound/WaveFile.h"

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


		private:
			static CResourceBankTable* m_instance;
			TRsourceBank<nsSound::CWaveFile> m_waveFileBank = {};
		};

	}
}