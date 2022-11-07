#include "YonejiDebugScene.h"
namespace nsAWA
{
	namespace nsScenes
	{
		bool CYonejiDebugScene::Start()
		{
			m_modelRenderer = NewGO<CModelRenderer>(EnGOPriority::enMid, "ynjTestModel");
			SModelInitData modelInitData;
			modelInitData.modelFilePath = "Assets/Models/hoge.fbx";
			modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);
			m_modelRenderer->SetScale(10.0f);
			m_modelRenderer->Init(modelInitData);

			return true;
		}

		void CYonejiDebugScene::OnDestroy()
		{
			DeleteGO(m_modelRenderer);
			return;
		}

		void CYonejiDebugScene::Update(float deltaTime)
		{
			nsMath::CVector3 addPos = { 0.0f, 0.0f, 0.0f };
			//if (Gamepad()->IsTrigger(EnPadButton::enA))
			//if (Keyboard()->IsTrigger(EnKeyButton::enA))
			if (Input()->IsTrigger(EnActionMapping::enJump))
			{
				addPos.y += 1.0f;
			}
			//if (Gamepad()->IsPress(EnPadButton::enB))
			//if (Keyboard()->IsPress(EnKeyButton::enB))
			if (Input()->IsPress(EnActionMapping::enAction))
			{
				addPos.y -= 0.5f;
			}

			addPos.z += Input()->GetVirtualAxis(EnAxisMapping::enForward) * 0.1f;
			addPos.x += Input()->GetVirtualAxis(EnAxisMapping::enRight) * 0.1f;
			auto& camPos = MainCamera()->GetPosition();
			//MainCamera()->SetPosition(camPos + addPos);
			auto& modelPos = m_modelRenderer->GetPosition();
			m_modelRenderer->SetPosition(modelPos + addPos);

			return;
		}
	}
}