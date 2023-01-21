#include "LoadAsynchronousSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CLoadAsynchronousSample::m_kModelFilePath =
			"Assets/Models/player.fbx";
		const char* CLoadAsynchronousSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
			"Assets/Animations/Samples/Rumba_Dancing.fbx",
			"Assets/Animations/Samples/Hip_Hop_Dancing.fbx",
			"Assets/Animations/Samples/Robot_Hip_Hop_Dance.fbx",
		};


		bool CLoadAsynchronousSample::Start()
		{
			{
				SModelInitData modelInitData;
				modelInitData.modelFilePath = m_kModelFilePath;
				modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);
				modelInitData.animInitData.Init(
					static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);
				modelInitData.textureRootPath = "Player";
				modelInitData.SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);

				m_modelRenderer = NewGO<CModelRenderer>();
				m_modelRenderer->SetPosition({ 0.0f, 4.0f, 0.0f });
				m_modelRenderer->SetScale(0.1f);
				m_modelRenderer->Init(modelInitData);
				m_modelRenderer->SetIsAnimationLoop(false);
			}

			{
				SModelInitData modelInitData;
				modelInitData.modelFilePath = "Assets/Models/Samples/TestPillar1.fbx";
				modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);

				m_boxMR = NewGO<CModelRenderer>();
				m_boxMR->SetPosition({ 0.0f, 0.0f, 0.0f });
				m_boxMR->SetScale(0.1f);
				m_boxMR->Init(modelInitData);
				m_boxMR->SetIsAnimationLoop(false);
			}

			MainCamera()->SetFarClip(10000.0f);

			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(MainCamera()->GetPosition());

			return true;
		}

		void CLoadAsynchronousSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);
			DeleteGO(m_boxMR);
			DeleteGO(m_modelRenderer);
			return;
		}

		void CLoadAsynchronousSample::Update(float deltaTime)
		{
			m_simpleMover->MoveCameraBySimpleMover();
			return;
		}


	}
}