#include "Gamepad.h"

namespace nsYMEngine
{
	namespace nsInput
	{
		void CGamepad::SInputState::Clear() noexcept
		{
			memset(this, 0, sizeof(*this));
			return;
		}

		const float CGamepad::m_kMaxMagnitudeForStick = SHRT_MAX;
		const DWORD CGamepad::m_kLRTriggerForXPadTable = 0;
		const DWORD CGamepad::m_kXPadTable[static_cast<int>(EnPadButton::enNumButtons)]
		{
			0,
			XINPUT_GAMEPAD_DPAD_UP,			// enUp
			XINPUT_GAMEPAD_DPAD_DOWN,		// enDown
			XINPUT_GAMEPAD_DPAD_LEFT,		// enLeft
			XINPUT_GAMEPAD_DPAD_RIGHT,		// enRight
			XINPUT_GAMEPAD_START,			// enStart
			XINPUT_GAMEPAD_BACK,			// enSelect
			XINPUT_GAMEPAD_LEFT_SHOULDER,	// enLB1
			m_kLRTriggerForXPadTable,		// enLB2	// トリガーは別のところで調べる
			XINPUT_GAMEPAD_LEFT_THUMB,		// enLB3
			XINPUT_GAMEPAD_RIGHT_SHOULDER,	// enRB1
			m_kLRTriggerForXPadTable,		// enRB2	// トリガーは別のところで調べる
			XINPUT_GAMEPAD_RIGHT_THUMB,		// enRB3
			XINPUT_GAMEPAD_A,				// enA
			XINPUT_GAMEPAD_B,				// enB
			XINPUT_GAMEPAD_X,				// enX
			XINPUT_GAMEPAD_Y				// enY
		};


		void CGamepad::CheckConnectionStatus() noexcept
		{
			// XInputGetState()の前に、ステートをクリアする。
			ClearState();

			// XInputからコントローラーの情報を取得
			auto dwResult = XInputGetState(m_userIndex, &m_xState);

			if (dwResult == ERROR_SUCCESS)
			{
				// コントローラーが接続されています。
				m_connectionStatus = EnConnectionStatus::enConntected;
			}
			else
			{
				// コントローラーが接続されていません。
				m_connectionStatus = EnConnectionStatus::enCheckedConnection;
				if (m_isDirty)
				{
					// 値が変更されてから接続が切れたら、1回クリアする必要がある。
					// クリアしないと、最後の入力が残り続けて、ずっと入力がある状態になってしまう。
					m_inputState.Clear();
					m_isDirty = false;
				}
			}

			return;
		}


		void CGamepad::Update() noexcept
		{
			if (m_connectionStatus != EnConnectionStatus::enConntected)
			{
				// 接続されていない。
				return;
			}

			if (m_xState.dwPacketNumber == m_prevPackedNumber)
			{
				// 前回の更新から変化なし。
				// 更新はなくても、トリガーフラグだけはリセットする。
				m_inputState.triggerArray.reset();
				return;
			}

			// パケット番号更新。
			m_prevPackedNumber = m_xState.dwPacketNumber;
			// 更新が入ったため、接続が切れたときに1回クリアする。
			m_isDirty = true;

			// ボタンの入力を調べる
			ExamineButtonInput();

			// 左スティックの入力を調べる
			ExamineStickInput(
				&m_inputState.lStick,
				&m_inputState.lStickMagnitude,
				m_xState.Gamepad.sThumbLX,
				m_xState.Gamepad.sThumbLY,
				XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
			);

			// 右スティックの入力を調べる
			ExamineStickInput(
				&m_inputState.rStick,
				&m_inputState.rStickMagnitude,
				m_xState.Gamepad.sThumbRX,
				m_xState.Gamepad.sThumbRY,
				XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
			);



			return;
		}

		void CGamepad::ClearState()
		{
			ZeroMemory(&m_xState, sizeof(XINPUT_STATE));
			return;
		}

		void CGamepad::ExamineButtonInput()
		{
			// トリガーを除く、各ボタンの状態を調べる。
			for (int i = 0; i < static_cast<int>(EnPadButton::enNumButtons); i++)
			{
				if (m_kXPadTable[i] == m_kLRTriggerForXPadTable)
				{
					// トリガーは、ここで更新しない。この後のトリガー用の処理で更新する。
					// ここで更新すると、pressが毎回0になるから、
					// この後のトリガー用の更新でtriggerが毎回1になってしまう。
					continue;
				}

				if (m_xState.Gamepad.wButtons & m_kXPadTable[i])
				{
					// 対象のボタンが押されている。
					// triggerは、ボタンが押された最初の1フレームだけ1にする。
					// pressは、ボタンが押されている間ずっと1にする。
					m_inputState.triggerArray[i] = 1 ^ m_inputState.pressArray[i];
					m_inputState.pressArray[i] = 1;
				}
				else
				{
					// 対象のボタンが押されていない。
					m_inputState.triggerArray[i] = 0;
					m_inputState.pressArray[i] = 0;
				}
			}

			// トリガーの状態を調べる。

			constexpr int idxLB2 = static_cast<int>(EnPadButton::enLB2);
			constexpr int idxRB2 = static_cast<int>(EnPadButton::enRB2);
			// 左トリガーを調べる。
			if (m_xState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				// 押されている。
				m_inputState.triggerArray[idxLB2] = 1 ^ m_inputState.pressArray[idxLB2];
				m_inputState.pressArray[idxLB2] = 1;
			}
			else
			{
				// 押されていない。
				m_inputState.triggerArray[idxLB2] = 0;
				m_inputState.pressArray[idxLB2] = 0;
			}
			// 右トリガーを調べる。
			if (m_xState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				// 押されている。
				m_inputState.triggerArray[idxRB2] = 1 ^ m_inputState.pressArray[idxRB2];
				m_inputState.pressArray[idxRB2] = 1;
			}
			else
			{
				// 押されていない。
				m_inputState.triggerArray[idxRB2] = 0;
				m_inputState.pressArray[idxRB2] = 0;
			}

			return;
		}


		void CGamepad::ExamineStickInput(
			nsMath::CVector2* pStick,
			float* pStickMagnitude,
			const float kThumbX,
			const float kThumbY,
			const float kDeadZone)
		{
			// スティックの入力量
			float magnitude = sqrt(kThumbX * kThumbX + kThumbY * kThumbY);

			// スティックの入力量がデッドゾーンを超えているか？
			if (magnitude > kDeadZone)
			{
				// XYの入力量
				pStick->x = kThumbX / magnitude;
				pStick->y = kThumbY / magnitude;

				// 予想される最大値で、入力量をクリップする。
				magnitude = std::min(magnitude, m_kMaxMagnitudeForStick);

				// 入力量を、デッドゾーンからの相対的な大きさに調整する。
				magnitude -= kDeadZone;

				// 入力量を0.0f～1.0fに正規化する。
				*pStickMagnitude = magnitude /
					(m_kMaxMagnitudeForStick - kDeadZone);

			}
			else
			{
				// 入力量がデッドゾーン内の場合、入力量を0にする。
				magnitude = 0.0f;
				*pStickMagnitude = 0.0f;
				*pStick = nsMath::CVector2::Zero();
			}


			return;
		}


	}
}