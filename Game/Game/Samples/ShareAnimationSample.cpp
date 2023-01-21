#include "ShareAnimationSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CShareAnimationSample::m_kModelFilePath[m_kNumModels] =
		{
			"Assets/Models/player.fbx",
			"Assets/Models/Humans/Player1.fbx",
			"Assets/Models/Humans/Player1.fbx"
		};

		const char* CShareAnimationSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
			"Assets/Animations/Player/Sword_JumpAttack.fbx"
		};

		bool CShareAnimationSample::Start()
		{
			// CameraSetting
			MainCamera()->SetPosition({ 0.0f,10.0f,-20.0f });


			SModelInitData modelInitData[m_kNumModels];

			modelInitData[0].textureRootPath = "player";

			modelInitData[1].SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);

			modelInitData[2].SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);


			constexpr float distance = 5.0f;

			for (int i = 0; i < m_kNumModels; i++)
			{
				modelInitData[i].animInitData.Init(
					static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);
				modelInitData[i].vertexBias.SetRotationXDeg(90.0f);
				modelInitData[i].SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
				modelInitData[i].SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
				modelInitData[i].modelFilePath = m_kModelFilePath[i];

				m_modelRenderer[i] = NewGO<CModelRenderer>();
				float posX = -distance * static_cast<float>(m_kNumModels) / 2.0f;
				posX += distance * static_cast<float>(i);
				m_modelRenderer[i]->SetPosition({ posX ,0.0f, 0.0f });
				m_modelRenderer[i]->SetScale(0.1f);
				m_modelRenderer[i]->Init(modelInitData[i]);
			}

			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(MainCamera()->GetPosition());

			return true;
		}

		void CShareAnimationSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);

			for (int i = 0; i < m_kNumModels; i++)
			{
				DeleteGO(m_modelRenderer[i]);
			}

			return;
		}

		void CShareAnimationSample::Update(float deltaTime)
		{
			m_simpleMover->MoveCameraBySimpleMover();


			// ControlAnimations

			static int animIdx[m_kNumModels] = { 0,0 };

			if (Keyboard()->IsTrigger(EnKeyButton::enSpace))
			{
				for (int i = 0; i < m_kNumModels; i++)
				{
					if (m_modelRenderer[i]->IsLoadingAsynchronous())
					{
						continue;
					}

					animIdx[i]++;
					if (animIdx[i] >= static_cast<int>(EnAnimType::enNum))
					{
						animIdx[i] = 0;
					}
					m_modelRenderer[i]->PlayAnimation(animIdx[i]);
				}
			}




			return;
		}
	}
}