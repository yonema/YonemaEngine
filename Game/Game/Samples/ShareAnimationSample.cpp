#include "ShareAnimationSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CShareAnimationSample::m_kModelFilePath[m_kNumModels] =
		{
			"Assets/Models/Humans/Base.fbx",
			"Assets/Models/Humans/Player1.fbx",
			"Assets/Models/Humans/Player2.fbx",
			"Assets/Models/Humans/Player3.fbx",
			"Assets/Models/Humans/Goddess.fbx",
			"Assets/Models/Humans/Chris.fbx",
			"Assets/Models/Humans/Mary.fbx",
			"Assets/Models/Humans/Rachel.fbx",
			"Assets/Models/Humans/Barbara.fbx",
			"Assets/Models/Humans/Michelle.fbx",
			"Assets/Models/Humans/John.fbx",
			"Assets/Models/Humans/Alain.fbx",
			"Assets/Models/Humans/Sara.fbx",
			"Assets/Models/Humans/Tom.fbx",
			"Assets/Models/Humans/James.fbx",
			"Assets/Models/Humans/Kevin.fbx",
			"Assets/Models/Humans/Guardian.fbx",
			"Assets/Models/Humans/MobMan.fbx",
			"Assets/Models/Humans/MobWoman.fbx",
		};

		const char* CShareAnimationSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
			//"Assets/Animations/Player/Sword_JumpAttack.fbx"
		};

		bool CShareAnimationSample::Start()
		{
			EnableDebugDrawPhysicsLine();

			// CameraSetting
			MainCamera()->SetNearClip(0.01f);
			MainCamera()->SetFarClip(1000.0f);
			MainCamera()->SetPosition({ 0.0f,10.0f,-20.0f });


			SModelInitData modelInitData[m_kNumModels];

			constexpr float distance = 10.0f;

			for (int i = 0; i < m_kNumModels; i++)
			{
				if (i != 0)
				{
					modelInitData[i].SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);
				}
				modelInitData[i].animInitData.Init(
					static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);
				modelInitData[i].vertexBias.SetRotationXDeg(90.0f);
				modelInitData[i].SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
				modelInitData[i].SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
				modelInitData[i].SetFlags(EnModelInitDataFlags::enCullingOff);
				modelInitData[i].modelFilePath = m_kModelFilePath[i];
				modelInitData[i].retargetSkeltonName = "PlayerSkelton";
				modelInitData[i].distanceToReducingUpdate = 50.0f;

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

			//nsGeometries::CGeometryData::DebugUpdate(deltaTime);

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
					m_modelRenderer[i]->PlayAnimationFromMiddle(animIdx[i], 1.0f);
				}
			}




			return;
		}
	}
}