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
				nsGameWindow::MessageBoxError(L"InputManager��������������܂����B");
			}

			m_isInstantiated = true;
			// �ŏ��̓m�[�^�C���Őڑ��B
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
				// �ꉞ�A����`�F�b�N
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
			// ���t���[���S�Ẵp�b�h�̐ڑ��󋵂𒲂ׂ�̂ł͂Ȃ��A
			// �ڑ��p�b�h�͏�ɒ��ׁA
			// ��ڑ��p�b�h�͈��Ԋu���Ƃ�1�����ׂ�B

			// ���ɐڑ��󋵂𒲂ׂ��ڑ��p�b�h��T���B
			bool isAllChecked = MarkNextCheckConnection(deltaTime);

			if (isAllChecked)
			{
				// ��ڑ��p�b�h����ʂ蒲�׏I������A�N���A���Ă܂����ׂȂ����B
				ClearCheckedStatus();
			}

			// �ڑ��p�b�h�ƁA�}�[�N���ꂽ��ڑ��p�b�h�̐ڑ���Ԃ𒲂ׂ�B
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
			// (��)�p�t�H�[�}���X��̗��R����A���t���[���A�u��́v���[�U�[�X���b�g�ɑ΂���
			//  XInputGetState ���Ăяo���Ȃ��悤�ɂ��܂��傤�B
			// ����ɁA�V�����R���g���[���̃`�F�b�N�𐔕b�����ɍs�����Ƃ������߂��܂��B


			// ���b�����ɏ��������s����B
			if (m_timerToMarkNextCheckConnection < m_kTimeToMarkNextCheckConnection)
			{
				m_timerToMarkNextCheckConnection += deltaTime;
				return false;
			}

			m_timerToMarkNextCheckConnection = 0.0f;

			// �p�b�h�̒�����enDisconnected��Ԃ̃p�b�h��1�T�������āA
			// �ڑ��󋵂𒲂ׂ�}�[�N������B
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

			// enDisconnected��Ԃ̃p�b�h��1�������Ȃ�����Atrue��߂��B
			return true;
		}


		void CInputManager::ClearCheckedStatus()
		{
			// �`�F�b�N�ς݂̃X�e�[�^�X���N���A���āA�ēx�A�ڑ��𒲂ׂ�悤�ɂ���B

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