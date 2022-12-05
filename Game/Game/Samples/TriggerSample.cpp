#include "TriggerSample.h"
#include <PxPhysicsAPI.h>
#include "../../YonemaEngine/Physics/PhysicsEngine.h"
#include "../../YonemaEngine/YonemaEngine.h"

namespace nsAWA
{
	namespace nsSamples
	{
		//////////////// CDeathTrigger ////////////////
#pragma region CDeathTrigger
		bool CDeathTrigger::Start()
		{
			m_triggerBox.InitAsBox(
				{ 5.0f,30.0f,30.0f }, { -20.0f,0.0f,0.0f });

			// TriggerEnter�̃R�[���o�b�N�֐���ݒ�B
			m_triggerBox.SetOnTriggerEnterFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerEnter(otherData); });

			return true;
		}

		void CDeathTrigger::OnDestroy()
		{
			m_triggerBox.Release();
			return;
		}

		void CDeathTrigger::Update(float deltaTime)
		{
			if (Keyboard()->IsTrigger(EnKeyButton::en2))
			{
				if (m_triggerBox.IsActive())
				{
					m_triggerBox.Deactivate();
				}
				else
				{
					m_triggerBox.Activate();
				}
			}
			return;
		}

		void CDeathTrigger::OnTriggerEnter(CExtendedDataForRigidActor* otherData)
		{
			// �g���K�[�ɓ����Ă����I�u�W�F�N�g�̌^��<CRigidPlayer>�������炻�̃I�u�W�F�N�g���A
			// �Ⴄ�^��������nullptr���Ԃ��Ă���B
			auto* rPlayer = otherData->GetOwner<CRigidPlayer>();
			if (rPlayer == nullptr)
			{
				return;
			}

			DeleteGO(rPlayer);

			return;
		}
#pragma endregion CDeathTrigger


		//////////////// CWallTrigger ////////////////
#pragma region CDeathWallTrigger
		bool CWallTrigger::Start()
		{
			// ����������t���ď������B
			// ���������ɂ��ẮAGame/PhysicsAttributes.h���Q�ƁB
			m_triggerBox.InitAsBox(
				{ 5.0f,30.0f,30.0f }, { 20.0f,0.0f,0.0f }, EnPhysicsAttributes::enWall);
			return true;
		}

		void CWallTrigger::OnDestroy()
		{
			m_triggerBox.Release();
			return;
		}

		void CWallTrigger::Update(float deltaTime)
		{
			if (Keyboard()->IsTrigger(EnKeyButton::en3))
			{
			if (m_triggerBox.IsActive())
			{
				m_triggerBox.Deactivate();
			}
			else
			{
				m_triggerBox.Activate();
			}
			}
			return;
		}
#pragma endregion CDeathWallTrigger


		//////////////// CRigidPlayer ////////////////
#pragma region CRigidPlayer
		bool CRigidPlayer::Start()
		{
			m_dynamicSphere.InitAsSphere(1.0f, { 0.0f,20.0f,0.0f });

			// �g���K�[��Owner(������)���w�肵�ď������B
			// Owner�̌^���ƃ|�C���^��ݒ肷��B
			m_triggerBox.InitAsBox<CRigidPlayer>(this, { 1.5f,1.5f,1.5f }, { 0.0f, 20.0f, 0.0f });

			// �e��R�[���o�b�N�֐��̐ݒ�
			m_triggerBox.SetOnTriggerEnterFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerEnter(otherData); }
			);
			m_triggerBox.SetOnTriggerStayFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerStay(otherData); }
			);
			m_triggerBox.SetOnTriggerExitFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerExit(otherData); }
			);

			// �m�F�p�̃t�H���g�����_���[�̏�����
			m_enterCounterFR = NewGO<CFontRenderer>();
			m_stayTimerFR = NewGO<CFontRenderer>();
			m_exitCounterFR = NewGO<CFontRenderer>();

			SFontParameter fontParam;
			fontParam.anchor = EnAnchors::enMiddleRight;
			fontParam.pivot = { 1.0f,0.5f };
			fontParam.scale = 0.5f;

			fontParam.text = L"�ǂɓ������񐔁@0��";
			fontParam.position = { -50.0f, -50.0f };
			m_enterCounterFR->Init(fontParam);

			fontParam.text = L"�ǂɓ����Ă���b���@0.0000�b";
			fontParam.position = { -50.0f, 0.0f };
			m_stayTimerFR->Init(fontParam);

			fontParam.text = L"�ǂ���o���񐔁@0��";
			fontParam.position = { -50.0f, 50.0f };
			m_exitCounterFR->Init(fontParam);

			return true;
		}

		void CRigidPlayer::OnDestroy()
		{
			DeleteGO(m_enterCounterFR);
			DeleteGO(m_stayTimerFR);
			DeleteGO(m_exitCounterFR);

			m_triggerBox.Release();
			m_dynamicSphere.Release();
			return;
		}

		void CRigidPlayer::Update(float deltaTime)
		{
			if (Keyboard()->IsTrigger(EnKeyButton::en1))
			{
				if (m_triggerBox.IsActive() || m_dynamicSphere.IsActive())
				{
					m_triggerBox.Deactivate();
					m_dynamicSphere.Deactivate();
				}
				else
				{
					m_triggerBox.Activate();
					m_dynamicSphere.Activate();
				}
			}

			// DynamicObject�𓮂���

			constexpr float forcePower = 10.0f;
			if (Keyboard()->IsTrigger(EnKeyButton::enLeft))
			{
				m_dynamicSphere.AddForce(nsMath::CVector3::Left() * forcePower);
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enRight))
			{
				m_dynamicSphere.AddForce(nsMath::CVector3::Right() * forcePower);
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enUp))
			{
				m_dynamicSphere.AddForce(nsMath::CVector3::Up() * forcePower);
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enDown))
			{
				m_dynamicSphere.AddForce(nsMath::CVector3::Down() * forcePower);
			}

			// DynamicObject�ɍ��킹�āATrigger�𓮂����B

			const auto& pos = m_dynamicSphere.GetPosition();
			const auto& rot = m_dynamicSphere.GetRotation();
			m_triggerBox.SetPositionAndRotation(pos, rot);

			return;
		}

		void CRigidPlayer::OnTriggerEnter(CExtendedDataForRigidActor* otherData)
		{
			if (otherData->GetPhysicsAttribute() != EnPhysicsAttributes::enWall)
			{
				return;
			}

			// �Ǒ����̃g���K�[�̒��ɓ������u�ԂɎ��s�����B

			m_enterCounter++;

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_enterCounterWChar, dispTextSize, L"�ǂɓ������񐔁@%d��", m_enterCounter);
			m_enterCounterFR->SetText(m_enterCounterWChar);

			return;
		}
		void CRigidPlayer::OnTriggerStay(CExtendedDataForRigidActor* otherData)
		{
			if (otherData->GetPhysicsAttribute() != EnPhysicsAttributes::enWall)
			{
				return;
			}

			// �Ǒ����̃g���K�[�̒��ɓ����Ă���ԁA���s�����B

			m_stayTimer += CYonemaEngine::GetInstance()->GetDeltaTime();

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_stayTimerWChar, dispTextSize, L"�ǂɓ����Ă���b���@%.4f�b", m_stayTimer);
			m_stayTimerFR->SetText(m_stayTimerWChar);

			return;
		}
		void CRigidPlayer::OnTriggerExit(CExtendedDataForRigidActor* otherData)
		{
			if (otherData->GetPhysicsAttribute() != EnPhysicsAttributes::enWall)
			{
				return;
			}

			// �Ǒ����̃g���K�[����o�Č������u�ԂɎ��s������

			m_exitCounter++;

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_exitCounterWChar, dispTextSize, L"�ǂ���o���񐔁@%d��", m_exitCounter);
			m_exitCounterFR->SetText(m_exitCounterWChar);

			return;
		}
#pragma endregion CRigidPlayer


		//////////////// CTriggerSample ////////////////
#pragma region CTriggerSample
		bool CTriggerSample::Start()
		{
			EnableDebugDrawPhysicsLine();
			SetCullingBoxForDebugDrawLine(100.0f, nsMath::CVector3::Zero());

			// �����e�X�g�p�̃I�u�W�F�N�g�𐶐�
			m_deathTrigger = NewGO<CDeathTrigger>();
			m_wallTrigger = NewGO<CWallTrigger>();
			m_rigidPlayer = NewGO<CRigidPlayer>();
			
			// ���ƂȂ�v���[���𐶐�

			nsMath::CVector3 nVec = { 0.0f,1.0f,0.0f };
			nVec.Normalize();
			m_staticPlane.InitAsPlane(nVec, 0.0f);
			m_staticPlane.SetPosition({ 0.0f,5.0f,0.0f });

			return true;
		}

		void CTriggerSample::OnDestroy()
		{
			DeleteGO(m_deathTrigger);
			DeleteGO(m_wallTrigger);
			DeleteGO(m_rigidPlayer);

			m_staticPlane.Release();

			return;
		}

		void CTriggerSample::Update(float deltaTime)
		{
			return;
		}
#pragma endregion CTriggerSample

	}
}