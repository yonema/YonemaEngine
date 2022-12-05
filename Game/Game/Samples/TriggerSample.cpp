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

			// TriggerEnterのコールバック関数を設定。
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
			// トリガーに入ってきたオブジェクトの型が<CRigidPlayer>だったらそのオブジェクトが、
			// 違う型だったらnullptrが返ってくる。
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
			// 物理属性を付けて初期化。
			// 物理属性については、Game/PhysicsAttributes.hを参照。
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

			// トリガーのOwner(持ち主)を指定して初期化。
			// Ownerの型情報とポインタを設定する。
			m_triggerBox.InitAsBox<CRigidPlayer>(this, { 1.5f,1.5f,1.5f }, { 0.0f, 20.0f, 0.0f });

			// 各種コールバック関数の設定
			m_triggerBox.SetOnTriggerEnterFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerEnter(otherData); }
			);
			m_triggerBox.SetOnTriggerStayFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerStay(otherData); }
			);
			m_triggerBox.SetOnTriggerExitFunc(
				[&](CExtendedDataForRigidActor* otherData) {OnTriggerExit(otherData); }
			);

			// 確認用のフォントレンダラーの初期化
			m_enterCounterFR = NewGO<CFontRenderer>();
			m_stayTimerFR = NewGO<CFontRenderer>();
			m_exitCounterFR = NewGO<CFontRenderer>();

			SFontParameter fontParam;
			fontParam.anchor = EnAnchors::enMiddleRight;
			fontParam.pivot = { 1.0f,0.5f };
			fontParam.scale = 0.5f;

			fontParam.text = L"壁に入った回数　0回";
			fontParam.position = { -50.0f, -50.0f };
			m_enterCounterFR->Init(fontParam);

			fontParam.text = L"壁に入っている秒数　0.0000秒";
			fontParam.position = { -50.0f, 0.0f };
			m_stayTimerFR->Init(fontParam);

			fontParam.text = L"壁から出た回数　0回";
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

			// DynamicObjectを動かす

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

			// DynamicObjectに合わせて、Triggerを動かす。

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

			// 壁属性のトリガーの中に入った瞬間に実行される。

			m_enterCounter++;

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_enterCounterWChar, dispTextSize, L"壁に入った回数　%d回", m_enterCounter);
			m_enterCounterFR->SetText(m_enterCounterWChar);

			return;
		}
		void CRigidPlayer::OnTriggerStay(CExtendedDataForRigidActor* otherData)
		{
			if (otherData->GetPhysicsAttribute() != EnPhysicsAttributes::enWall)
			{
				return;
			}

			// 壁属性のトリガーの中に入っている間、実行される。

			m_stayTimer += CYonemaEngine::GetInstance()->GetDeltaTime();

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_stayTimerWChar, dispTextSize, L"壁に入っている秒数　%.4f秒", m_stayTimer);
			m_stayTimerFR->SetText(m_stayTimerWChar);

			return;
		}
		void CRigidPlayer::OnTriggerExit(CExtendedDataForRigidActor* otherData)
		{
			if (otherData->GetPhysicsAttribute() != EnPhysicsAttributes::enWall)
			{
				return;
			}

			// 壁属性のトリガーから出て言った瞬間に実行させる

			m_exitCounter++;

			constexpr size_t dispTextSize = sizeof(wchar_t) * static_cast<size_t>(64);
			StringCbPrintf(m_exitCounterWChar, dispTextSize, L"壁から出た回数　%d回", m_exitCounter);
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

			// 物理テスト用のオブジェクトを生成
			m_deathTrigger = NewGO<CDeathTrigger>();
			m_wallTrigger = NewGO<CWallTrigger>();
			m_rigidPlayer = NewGO<CRigidPlayer>();
			
			// 床となるプレーンを生成

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