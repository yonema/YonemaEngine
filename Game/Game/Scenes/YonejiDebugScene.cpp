#include "YonejiDebugScene.h"
namespace nsAWA
{
	namespace nsScenes
	{
		bool CYonejiDebugScene::Start()
		{
			//m_modelRenderer = NewGO<CModelRenderer>(EnGOPriority::enMid, "ynjTestModel");
			m_modelRenderer = NewGO<CModelRenderer>("ynjTestModel");
			SModelInitData modelInitData;
			//modelInitData.modelFilePath = "Assets/Models/hoge.fbx";
			//modelInitData.modelFilePath = "Assets/Animations/Rumba_Dancing.fbx";
			//modelInitData.modelFilePath = "Assets/Models/testWoldPosAndRotate.fbx";
			//modelInitData.modelFilePath = "Assets/Models/skeltalBox.fbx";
			//modelInitData.modelFilePath = "Assets/Models/Giyara.fbx";
			modelInitData.modelFilePath = "Assets/Models/SampleBox.fbx";
			//modelInitData.modelFilePath = "Assets/Models/unitychan.fbx";
			modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);

			constexpr unsigned int kAnimNum = 3;
			const char* animfilePaths[kAnimNum] = {
				"Assets/Animations/Rumba_Dancing.fbx",
				"Assets/Animations/Hip_Hop_Dancing.fbx",
				"Assets/Animations/Robot_Hip_Hop_Dance.fbx"
			};
			SAnimationInitData animInitData(kAnimNum, animfilePaths);
			//modelInitData.animInitData = &animInitData;
			m_modelRenderer->SetScale(0.1f);
			//m_modelRenderer->SetScale(10.0f);
			m_modelRenderer->SetPosition({ 0.0f, 4.0f, 0.0f });
			m_modelRenderer->Init(modelInitData);
			m_modelRenderer->SetIsAnimationLoop(false);



			SSpriteInitData sampleSpriteData;
			sampleSpriteData.filePath = "Assets/Images/200x200PNG.png";
			sampleSpriteData.spriteSize = { 300.0f,300.0f };


			return true;
		}

		void CYonejiDebugScene::OnDestroy()
		{
			DeleteGO(m_modelRenderer);
			return;
		}

		void CYonejiDebugScene::Update(float deltaTime)
		{
			if (Keyboard()->IsPress(EnKeyButton::enNone))
			{
				int a = 1;
			}
			nsMath::CVector3 addPos = { 0.0f, 0.0f, 0.0f };
			//if (Gamepad()->IsTrigger(EnPadButton::enA))
			//if (Keyboard()->IsTrigger(EnKeyButton::enA))
			if (Input()->IsTrigger(EnActionMapping::enJump) || m_modelRenderer->IsPlaying() != true)
			{
				//addPos.y += 1.0f;
				static unsigned int animIdx = 0;
				animIdx++;
				if (animIdx > 2)
				{
					animIdx = 0;
				}
				float animSpeed[3] = {1.0f,1.5f,2.0f};
				static bool isAnimLoop = false;
				isAnimLoop = !isAnimLoop;

				m_modelRenderer->PlayAnimation(animIdx, animSpeed[animIdx], isAnimLoop);
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

			auto rot = m_modelRenderer->GetRotation();
			rot.AddRotationYDeg(50.0f * deltaTime);
			m_modelRenderer->SetRotation(rot);


			return;
		}
	}
}