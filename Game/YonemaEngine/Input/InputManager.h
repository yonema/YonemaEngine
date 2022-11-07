#pragma once
#include "Gamepad.h"
#include "Keyboard.h"
#include "../../Game/InputMapping.h"

namespace nsYMEngine
{
	namespace nsInput
	{
		class CInputManager : private nsUtils::SNoncopyable
		{
		public:
			struct SInputState
			{
				std::bitset<static_cast<int>(EnActionMapping::enNumActions)> pressArray;
				std::bitset<static_cast<int>(EnActionMapping::enNumActions)> triggerArray;
			};

		private:
			static const DWORD m_kMaxNumGamePads = 4;
			static const float m_kTimeToMarkNextCheckConnection;


		public:
			CInputManager();
			~CInputManager();

			void Update(float deltaTime);

			constexpr const CGamepad* const GetGamepad(int userIdx) const noexcept
			{
				return m_gamepads[userIdx];
			}

			constexpr const CKeyboard* const GetKeyboard() const noexcept
			{
				return m_keyboard;
			}

			constexpr bool IsTrigger(EnActionMapping action) const noexcept
			{
				return m_virtualInputState.triggerArray[static_cast<int>(action)];
			}

			constexpr bool IsPress(EnActionMapping action) const noexcept
			{
				return m_virtualInputState.pressArray[static_cast<int>(action)];
			}
			inline bool IsPressAnyKey() const noexcept
			{
				return m_virtualInputState.pressArray.any();
			}

			constexpr float GetVirtualAxis(EnAxisMapping axis) const noexcept
			{
				return m_virtualAxis[static_cast<int>(axis)];
			}

			constexpr void SetUserIndex(unsigned int userIndex) noexcept
			{
				m_userIndex = userIndex;
			}

			constexpr int GetUserIndex() const noexcept
			{
				return m_userIndex;
			}

		private:
			void CheckConnectionStatus(float deltaTime);

			bool MarkNextCheckConnection(float deltaTime);

			void ClearCheckedStatus();

			void ExecuteInputUpdates();


		private:
			static bool m_isInstantiated;
			unsigned int m_userIndex = 0;
			CGamepad* m_gamepads[m_kMaxNumGamePads] = { nullptr };
			CKeyboard* m_keyboard = nullptr;
			float m_timerToMarkNextCheckConnection = 0.0f;

			SInputState m_virtualInputState = {};
			float m_virtualAxis[static_cast<int>(EnAxisMapping::enNumAxes)] = { 0.0f };

		};

	}
}