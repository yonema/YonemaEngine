#include "SkeltalAnimationSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CSkeltalAnimationSample::m_kModelFilePath =
			"Assets/Animations/Rumba_Dancing.fbx";
			//"Assets/Models/unitychan.fbx";
		const char* CSkeltalAnimationSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Rumba_Dancing.fbx",
			"Assets/Animations/Hip_Hop_Dancing.fbx",
			"Assets/Animations/Robot_Hip_Hop_Dance.fbx",
		};


		bool CSkeltalAnimationSample::Start()
		{
			SAnimationInitData animInitData(
				static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);

			SModelInitData modelInitData;
			modelInitData.modelFilePath = m_kModelFilePath;
			//modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);
			modelInitData.animInitData = &animInitData;

			m_modelRenderer = NewGO<CModelRenderer>("SkeltalAnimationSampleModel");
			m_modelRenderer->SetPosition({ 0.0f, 4.0f, 0.0f });
			m_modelRenderer->SetScale(0.1f);
			m_modelRenderer->Init(modelInitData);
			m_modelRenderer->SetIsAnimationLoop(false);

			return true;
		}

		void CSkeltalAnimationSample::OnDestroy()
		{
			DeleteGO(m_modelRenderer);
			return;
		}

		void CSkeltalAnimationSample::Update(float deltaTime)
		{
			bool isTrigger1 =
				Gamepad()->IsTrigger(EnPadButton::enA) ||
				Keyboard()->IsTrigger(EnKeyButton::enSpace);
			bool isTriggerUp =
				Gamepad()->IsTrigger(EnPadButton::enUp) ||
				Keyboard()->IsTrigger(EnKeyButton::enUp);
			bool isTriggerDown =
				Gamepad()->IsTrigger(EnPadButton::enDown) ||
				Keyboard()->IsTrigger(EnKeyButton::enDown);
			bool isTrigger2 =
				Gamepad()->IsTrigger(EnPadButton::enB) ||
				Keyboard()->IsTrigger(EnKeyButton::enEnter);

			static bool isAnimLoop = false;
			static float animSpeed = 1.0f;
			static unsigned int animIdx = 0;

			// アニメーション切り替え
			if (isTrigger1 || m_modelRenderer->IsPlaying() != true)
			{
				animIdx++;
				if (animIdx >= static_cast<unsigned int>(EnAnimType::enNum))
				{
					animIdx = 0;
				}
			}
			// アニメーションスピード変更
			if (isTriggerUp)
			{
				animSpeed += 0.5f;
				if (animSpeed > 3.0f)
				{
					animSpeed = 3.0f;
				}
			}
			else if (isTriggerDown)
			{
				animSpeed -= 0.5f;
				if (animSpeed < 0.5f)
				{
					animSpeed = 0.5f;
				}
			}
			// ループ再生切り替え
			if (isTrigger2)
			{
				isAnimLoop = !isAnimLoop;
			}

			m_modelRenderer->PlayAnimation(animIdx, animSpeed, isAnimLoop);

			// モデル回転
			auto rotTmp = m_modelRenderer->GetRotation();
			rotTmp.AddRotationYDeg(50.0f * deltaTime);
			m_modelRenderer->SetRotation(rotTmp);

			return;
		}
	}
}