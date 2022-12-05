#include "SimpleMover.h"
#include "../Input/InputManager.h"

namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		bool CSimpleMover::Start()
		{
			return true;
		}
		void CSimpleMover::OnDestroy()
		{
			return;
		}
		void CSimpleMover::Update(float deltaTime)
		{
			CalcForward();
			CalcRight();

			Turn(deltaTime);
			Move(deltaTime);

			return;
		}

		void CSimpleMover::CalcForward() noexcept 
		{
			m_forward = nsMath::CVector3::Front();
			m_rotation.Apply(m_forward);
			m_forward.Normalize();
			return;
		}

		void CSimpleMover::CalcRight() noexcept
		{
			m_right.Cross(nsMath::CVector3::Up(), GetForward());
			m_right.Normalize();
			return;
		}

		void CSimpleMover::Turn(float deltaTime) noexcept
		{
			// ���͒l�擾
			auto* keyboard = CYonemaEngine::GetInstance()->GetInput()->GetKeyboard();
			float inputTurnRight = 
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enLeft) ? -1.0f : 0.0f;
			inputTurnRight += 
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enRight) ? 1.0f : 0.0f;
			float inputTurnUp = 
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enUp) ? 1.0f : 0.0f;
			inputTurnUp +=
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enDown) ? -1.0f : 0.0f;

			float turnSpeed = m_turnSpeed * deltaTime;

			const auto forward = GetForward();
			
			// Y��(������)��]

			// ���݂̉������̊p�x���v�Z
			float horizontalAngle = nsMath::RadToDeg(atan2(forward.x, forward.z));
			nsMath::CQuaternion horizontalRot;
			horizontalAngle += inputTurnRight * turnSpeed;
			horizontalRot.SetRotationYDeg(horizontalAngle);


			// ���[�J��X��(�c����)��]

			auto forwardXZ = GetForward();
			forwardXZ.y = 0.0f;
			float forwardXZLen = forwardXZ.Length();
			// ���݂̏c�����̊p�x���v�Z
			float verticalAngle = nsMath::RadToDeg(atan2(-forward.y, forwardXZLen));
			verticalAngle += inputTurnUp * turnSpeed;
			// �p�x����
			verticalAngle = std::min(verticalAngle, 45.0f);
			verticalAngle = std::max(verticalAngle, -45.0f);

			auto axisRight = GetRight();
			axisRight.y = 0.0f;
			axisRight.Normalize();
			nsMath::CQuaternion verticalRot;
			verticalRot.SetRotationDeg(axisRight, verticalAngle);

			m_rotation.Multiply(horizontalRot, verticalRot);

			return;
		}

		void CSimpleMover::Move(float deltaTime) noexcept
		{
			// ���͒l�擾
			auto* keyboard = CYonemaEngine::GetInstance()->GetInput()->GetKeyboard();
			float inputMoveForward =
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enW) ? 1.0f : 0.0f;
			inputMoveForward +=
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enS) ? -1.0f : 0.0f;
			float inputMoveRight =
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enA) ? -1.0f : 0.0f;
			inputMoveRight +=
				keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enD) ? 1.0f : 0.0f;

			bool isMoveUp = keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enQ);
			bool isMoveDown = keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enE);

			bool isDush = keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enShift);
			bool isWalk = keyboard->IsPress(nsInput::CKeyboard::EnKeyButton::enCtrl);


			// �ړ�
			auto moveVec = nsMath::CVector3::Zero();
			float moveSpeed = m_moveSpeed * deltaTime;
			if (isDush)
			{
				moveSpeed *= 2.0f;
			}
			if (isWalk)
			{
				moveSpeed *= 0.5f;
			}

			// XZ���ʂł̈ړ�����������v�Z
			auto moveForward = GetForward();
			auto moveRight = GetRight();
			moveForward.y = 0.0f;
			moveRight.y = 0.0f;

			// XZ���ʂł̈ړ�
			moveVec += moveForward * inputMoveForward * moveSpeed;
			moveVec += moveRight * inputMoveRight * moveSpeed;

			// Y���c�ړ�
			if (isMoveUp)
			{
				moveVec.y += moveSpeed;
			}
			if (isMoveDown)
			{
				moveVec.y -= moveSpeed;
			}

			m_position += moveVec;

			return;
		}
	}
}