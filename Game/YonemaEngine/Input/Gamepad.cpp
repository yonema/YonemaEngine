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
			m_kLRTriggerForXPadTable,		// enLB2	// �g���K�[�͕ʂ̂Ƃ���Œ��ׂ�
			XINPUT_GAMEPAD_LEFT_THUMB,		// enLB3
			XINPUT_GAMEPAD_RIGHT_SHOULDER,	// enRB1
			m_kLRTriggerForXPadTable,		// enRB2	// �g���K�[�͕ʂ̂Ƃ���Œ��ׂ�
			XINPUT_GAMEPAD_RIGHT_THUMB,		// enRB3
			XINPUT_GAMEPAD_A,				// enA
			XINPUT_GAMEPAD_B,				// enB
			XINPUT_GAMEPAD_X,				// enX
			XINPUT_GAMEPAD_Y				// enY
		};


		void CGamepad::CheckConnectionStatus() noexcept
		{
			// XInputGetState()�̑O�ɁA�X�e�[�g���N���A����B
			ClearState();

			// XInput����R���g���[���[�̏����擾
			auto dwResult = XInputGetState(m_userIndex, &m_xState);

			if (dwResult == ERROR_SUCCESS)
			{
				// �R���g���[���[���ڑ�����Ă��܂��B
				m_connectionStatus = EnConnectionStatus::enConntected;
			}
			else
			{
				// �R���g���[���[���ڑ�����Ă��܂���B
				m_connectionStatus = EnConnectionStatus::enCheckedConnection;
				if (m_isDirty)
				{
					// �l���ύX����Ă���ڑ����؂ꂽ��A1��N���A����K�v������B
					// �N���A���Ȃ��ƁA�Ō�̓��͂��c�葱���āA�����Ɠ��͂������ԂɂȂ��Ă��܂��B
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
				// �ڑ�����Ă��Ȃ��B
				return;
			}

			if (m_xState.dwPacketNumber == m_prevPackedNumber)
			{
				// �O��̍X�V����ω��Ȃ��B
				// �X�V�͂Ȃ��Ă��A�g���K�[�t���O�����̓��Z�b�g����B
				m_inputState.triggerArray.reset();
				return;
			}

			// �p�P�b�g�ԍ��X�V�B
			m_prevPackedNumber = m_xState.dwPacketNumber;
			// �X�V�����������߁A�ڑ����؂ꂽ�Ƃ���1��N���A����B
			m_isDirty = true;

			// �{�^���̓��͂𒲂ׂ�
			ExamineButtonInput();

			// ���X�e�B�b�N�̓��͂𒲂ׂ�
			ExamineStickInput(
				&m_inputState.lStick,
				&m_inputState.lStickMagnitude,
				m_xState.Gamepad.sThumbLX,
				m_xState.Gamepad.sThumbLY,
				XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
			);

			// �E�X�e�B�b�N�̓��͂𒲂ׂ�
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
			// �g���K�[�������A�e�{�^���̏�Ԃ𒲂ׂ�B
			for (int i = 0; i < static_cast<int>(EnPadButton::enNumButtons); i++)
			{
				if (m_kXPadTable[i] == m_kLRTriggerForXPadTable)
				{
					// �g���K�[�́A�����ōX�V���Ȃ��B���̌�̃g���K�[�p�̏����ōX�V����B
					// �����ōX�V����ƁApress������0�ɂȂ邩��A
					// ���̌�̃g���K�[�p�̍X�V��trigger������1�ɂȂ��Ă��܂��B
					continue;
				}

				if (m_xState.Gamepad.wButtons & m_kXPadTable[i])
				{
					// �Ώۂ̃{�^����������Ă���B
					// trigger�́A�{�^���������ꂽ�ŏ���1�t���[������1�ɂ���B
					// press�́A�{�^����������Ă���Ԃ�����1�ɂ���B
					m_inputState.triggerArray[i] = 1 ^ m_inputState.pressArray[i];
					m_inputState.pressArray[i] = 1;
				}
				else
				{
					// �Ώۂ̃{�^����������Ă��Ȃ��B
					m_inputState.triggerArray[i] = 0;
					m_inputState.pressArray[i] = 0;
				}
			}

			// �g���K�[�̏�Ԃ𒲂ׂ�B

			constexpr int idxLB2 = static_cast<int>(EnPadButton::enLB2);
			constexpr int idxRB2 = static_cast<int>(EnPadButton::enRB2);
			// ���g���K�[�𒲂ׂ�B
			if (m_xState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				// ������Ă���B
				m_inputState.triggerArray[idxLB2] = 1 ^ m_inputState.pressArray[idxLB2];
				m_inputState.pressArray[idxLB2] = 1;
			}
			else
			{
				// ������Ă��Ȃ��B
				m_inputState.triggerArray[idxLB2] = 0;
				m_inputState.pressArray[idxLB2] = 0;
			}
			// �E�g���K�[�𒲂ׂ�B
			if (m_xState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				// ������Ă���B
				m_inputState.triggerArray[idxRB2] = 1 ^ m_inputState.pressArray[idxRB2];
				m_inputState.pressArray[idxRB2] = 1;
			}
			else
			{
				// ������Ă��Ȃ��B
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
			// �X�e�B�b�N�̓��͗�
			float magnitude = sqrt(kThumbX * kThumbX + kThumbY * kThumbY);

			// �X�e�B�b�N�̓��͗ʂ��f�b�h�]�[���𒴂��Ă��邩�H
			if (magnitude > kDeadZone)
			{
				// XY�̓��͗�
				pStick->x = kThumbX / magnitude;
				pStick->y = kThumbY / magnitude;

				// �\�z�����ő�l�ŁA���͗ʂ��N���b�v����B
				magnitude = std::min(magnitude, m_kMaxMagnitudeForStick);

				// ���͗ʂ��A�f�b�h�]�[������̑��ΓI�ȑ傫���ɒ�������B
				magnitude -= kDeadZone;

				// ���͗ʂ�0.0f�`1.0f�ɐ��K������B
				*pStickMagnitude = magnitude /
					(m_kMaxMagnitudeForStick - kDeadZone);

			}
			else
			{
				// ���͗ʂ��f�b�h�]�[�����̏ꍇ�A���͗ʂ�0�ɂ���B
				magnitude = 0.0f;
				*pStickMagnitude = 0.0f;
				*pStick = nsMath::CVector2::Zero();
			}


			return;
		}


	}
}