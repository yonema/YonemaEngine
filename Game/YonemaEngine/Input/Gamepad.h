#pragma once
#include <Xinput.h>

namespace nsYMEngine
{
	namespace nsInput
	{

		class CGamepad : private nsUtils::SNoncopyable
		{
		public:
			enum class EnConnectionStatus
			{
				enDisconnected,
				enConntected,
				enNextCheckConnection,
				enCheckedConnection
			};

			/**
			 * @brief ゲームパッドの仮想ボタン
			*/
			enum class EnPadButton
			{
				enNone,
				enUp,
				enDown,
				enLeft,
				enRight,
				enStart,
				enSelect,
				enLB1,
				enLB2,
				enLB3,
				enRB1,
				enRB2,
				enRB3,
				enA,
				enB,
				enX,
				enY,
				enNumButtons
			};

			/**
			 * @brief 入力情報
			*/
			struct SInputState
			{
				float lStickMagnitude = 0.0f;
				nsMath::CVector2 lStick = nsMath::CVector2::Zero();
				float rStickMagnitude = 0.0f;
				nsMath::CVector2 rStick = nsMath::CVector2::Zero();
				std::bitset<static_cast<int>(EnPadButton::enNumButtons)> pressArray;
				std::bitset<static_cast<int>(EnPadButton::enNumButtons)> triggerArray;

				void Clear() noexcept;
			};

		private:
			static const float m_kMaxMagnitudeForStick;
			static const DWORD m_kLRTriggerForXPadTable;
			/**
			 * @brief XINPUT_GAMEPADから仮想ボタンへの変換を行うテーブル
			*/
			static const DWORD m_kXPadTable[static_cast<int>(EnPadButton::enNumButtons)];

		public:
			constexpr CGamepad(DWORD userIndex) noexcept
				: m_userIndex(userIndex) {};

			~CGamepad() = default;

			void CheckConnectionStatus() noexcept;

			void Update() noexcept;

			/**
			 * @brief ゲームパッドの接続状況を得る
			 * @return 接続状況
			*/
			constexpr EnConnectionStatus GetConnectionStatus() const noexcept
			{
				return m_connectionStatus;
			}

			/**
			 * @brief 
			 * @param connectionStatus 
			 * @return 
			*/
			constexpr void SetConnectionStatus(EnConnectionStatus connectionStatus) noexcept
			{
				m_connectionStatus = connectionStatus;
			}

			constexpr bool IsValid() const noexcept
			{
				return m_connectionStatus == EnConnectionStatus::enConntected;
			}

			constexpr const auto& GetInputState() const noexcept
			{
				return m_inputState;
			}

			constexpr bool IsTrigger(EnPadButton gamepadButton) const noexcept
			{
				return m_inputState.triggerArray[static_cast<int>(gamepadButton)];
			}
			constexpr bool IsPress(EnPadButton gamepadButton) const noexcept
			{
				return m_inputState.pressArray[static_cast<int>(gamepadButton)];
			}
			inline bool IsPressAnyKey() const noexcept
			{
				return m_inputState.pressArray.any();
			}

			constexpr float GetLStickMagnitude() const noexcept
			{
				return m_inputState.lStickMagnitude;
			}
			constexpr const auto& GetLStick() const noexcept
			{
				return m_inputState.lStick;
			}
			constexpr float GetLStickX() const noexcept
			{
				return GetLStick().x;
			}
			constexpr float GetLStickY() const noexcept
			{
				return GetLStick().y;
			}

			constexpr float GetRStickMagnitude() const noexcept
			{
				return m_inputState.rStickMagnitude;
			}
			constexpr const auto& GetRStick() const noexcept
			{
				return m_inputState.rStick;
			}
			constexpr float GetRStickX() const noexcept
			{
				return GetRStick().x;
			}
			constexpr float GetRStickY() const noexcept
			{
				return GetRStick().y;
			}

		private:
			void ClearState();

			void ExamineButtonInput();

			void ExamineStickInput(
				nsMath::CVector2* pStick,
				float* pStickMagnitude,
				const float kThumbX,
				const float kThumbY,
				const float kDeadZone
			);

		private:
			DWORD m_userIndex = -1;
			DWORD m_prevPackedNumber = -1;
			XINPUT_STATE m_xState = {};
			EnConnectionStatus m_connectionStatus = EnConnectionStatus::enDisconnected;

			SInputState m_inputState = {};
			bool m_isDirty = false;
		};

	}
}