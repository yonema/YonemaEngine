#include "InputManager.h"
#include "../GameWindow/MessageBox.h"


namespace nsYMEngine
{
	namespace nsInput
	{
		bool CInputManager::m_isInstantiated = false;
		const float CInputManager::m_kTimeToMarkNextCheckConnection = 2.0f;

		CInputManager::CInputManager()
		{
			if (m_isInstantiated)
			{
				nsGameWindow::MessageBoxError(L"InputManagerが複数生成されました。");
			}

			m_isInstantiated = true;
			// 最初はノータイムで接続。
			m_timerToMarkNextCheckConnection = m_kTimeToMarkNextCheckConnection;

			for (DWORD i = 0; i < m_kMaxNumGamePads; i++)
			{
				m_gamepads[i] = new CGamepad(i);
			}
			m_keyboard = new CKeyboard();

			return;
		}

		CInputManager::~CInputManager()
		{
			if (m_keyboard)
			{
				delete m_keyboard;
			}
			for (DWORD i = 0; i < m_kMaxNumGamePads; i++)
			{
				// 一応、一個ずつチェック
				if (m_gamepads[i])
				{
					delete m_gamepads[i];
				}
			}

			return;
		}


		void CInputManager::Update(float deltaTime)
		{
			CheckConnectionStatus(deltaTime);

			ExecuteInputUpdates();

			m_keyboard->Update();

			for (int i = 0; i < static_cast<int>(EnActionMapping::enNumActions); i++)
			{
				bool triggerFlag = false;
				bool pressFlag = false;
				for (const auto& gamepadButton : g_kActionMappingTable[i].gamepadButtonTable)
				{
					triggerFlag |= m_gamepads[m_userIndex]->IsTrigger(gamepadButton);

					pressFlag |= m_gamepads[m_userIndex]->IsPress(gamepadButton);
				}
				if ((!triggerFlag) || (!pressFlag))
				{
					for (const auto& keyboardButton : g_kActionMappingTable[i].keyboardButtontable)
					{
						triggerFlag |= m_keyboard->IsTrigger(keyboardButton);

						pressFlag |= m_keyboard->IsPress(keyboardButton);
					}
				}
				m_virtualInputState.triggerArray[i] = triggerFlag;
				m_virtualInputState.pressArray[i] = pressFlag;
			}



			for (int i = 0; i < static_cast<int>(EnAxisMapping::enNumAxes); i++)
			{
				m_virtualAxis[i] = 0.0f;
				for (const auto& gamepadButton : g_kAxisMappingTable[i].gamepadButtonTable)
				{
					m_virtualAxis[i] += 
						m_gamepads[m_userIndex]->IsPress(gamepadButton.first) * gamepadButton.second;
				}
				for (const auto& keyboardButton : g_kAxisMappingTable[i].keyboardButtontable)
				{
					m_virtualAxis[i] +=
						m_keyboard->IsPress(keyboardButton.first) * keyboardButton.second;
				}
				for (const auto& gamepadStick : g_kAxisMappingTable[i].gamepadStickTable)
				{
					switch (gamepadStick.first)
					{
					case nsInputManagerData::EnGamepadStickAxis::enLStickX:
						m_virtualAxis[i] += m_gamepads[m_userIndex]->GetLStickX() * gamepadStick.second;
						break;
					case nsInputManagerData::EnGamepadStickAxis::enLStickY:
						m_virtualAxis[i] += m_gamepads[m_userIndex]->GetLStickY() * gamepadStick.second;
						break;
					case nsInputManagerData::EnGamepadStickAxis::enRStickX:
						m_virtualAxis[i] += m_gamepads[m_userIndex]->GetRStickX() * gamepadStick.second;
						break;
					case nsInputManagerData::EnGamepadStickAxis::enRStickY:
						m_virtualAxis[i] += m_gamepads[m_userIndex]->GetRStickY() * gamepadStick.second;
						break;
					}

				}

				if (m_virtualAxis[i] > 1.0f)
				{
					m_virtualAxis[i] = 1.0f;
				}
				else if (m_virtualAxis[i] < -1.0f)
				{
					m_virtualAxis[i] = -1.0f;
				}
			}

			return;
		}

		void CInputManager::CheckConnectionStatus(float deltaTime)
		{
			// 毎フレーム全てのパッドの接続状況を調べるのではなく、
			// 接続パッドは常に調べ、
			// 非接続パッドは一定間隔ごとに1つずつ調べる。

			// 次に接続状況を調べる非接続パッドを探す。
			bool isAllChecked = MarkNextCheckConnection(deltaTime);

			if (isAllChecked)
			{
				// 非接続パッドを一通り調べ終えたら、クリアしてまた調べなおす。
				ClearCheckedStatus();
			}

			// 接続パッドと、マークされた非接続パッドの接続状態を調べる。
			for (int i = 0; i < m_kMaxNumGamePads; i++)
			{
				auto connectionStatus = m_gamepads[i]->GetConnectionStatus();
				if (connectionStatus == CGamepad::EnConnectionStatus::enConntected ||
					connectionStatus == CGamepad::EnConnectionStatus::enNextCheckConnection)
				{
					m_gamepads[i]->CheckConnectionStatus();
				}
			}
			return;
		}

		bool CInputManager::MarkNextCheckConnection(float deltaTime)
		{
			// https://learn.microsoft.com/en-us/windows/win32/xinput/getting-started-with-xinput
			// For performance reasons, don't call XInputGetState for an 'empty' user slot every frame.
			// We recommend that you space out checks for new controllers every few seconds instead.
			// (訳)パフォーマンス上の理由から、毎フレーム、「空の」ユーザースロットに対して
			//  XInputGetState を呼び出さないようにしましょう。
			// 代わりに、新しいコントローラのチェックを数秒おきに行うことをお勧めします。


			// 数秒おきに処理を実行する。
			if (m_timerToMarkNextCheckConnection < m_kTimeToMarkNextCheckConnection)
			{
				m_timerToMarkNextCheckConnection += deltaTime;
				return false;
			}

			m_timerToMarkNextCheckConnection = 0.0f;

			// パッドの中からenDisconnected状態のパッドを1つ探しだして、
			// 接続状況を調べるマークをつける。
			for (int i = 0; i < m_kMaxNumGamePads; i++)
			{
				if (m_gamepads[i]->GetConnectionStatus() ==
					CGamepad::EnConnectionStatus::enDisconnected)
				{
					m_gamepads[i]->SetConnectionStatus(
						CGamepad::EnConnectionStatus::enNextCheckConnection);

					return false;
				}

			}

			// enDisconnected状態のパッドが1つも無くなったら、trueを戻す。
			return true;
		}


		void CInputManager::ClearCheckedStatus()
		{
			// チェック済みのステータスをクリアして、再度、接続を調べるようにする。

			for (int i = 0; i < m_kMaxNumGamePads; i++)
			{
				if (m_gamepads[i]->GetConnectionStatus() ==
					CGamepad::EnConnectionStatus::enCheckedConnection)
				{
					m_gamepads[i]->SetConnectionStatus(
						CGamepad::EnConnectionStatus::enDisconnected);
				}
			}

			return;
		}

		void CInputManager::ExecuteInputUpdates()
		{
			for (int i = 0; i < m_kMaxNumGamePads; i++)
			{
				m_gamepads[i]->Update();
			}

			return;
		}
	}
}