#include "LODSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const char* CLODSample::m_kModelFilePath[m_kNumModels] =
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
		const char* CLODSample::m_kLODModelFilePath = "Assets/Models/Humans/Player1_Low.fbx";
		//const char* CLODSample::m_kLODModelFilePath = "Assets/Models/Humans/Goddess.fbx";
		const char* CLODSample::
			m_kAnimFilePaths[static_cast<int>(EnAnimType::enNum)] =
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
			//"Assets/Animations/Player/Sword_JumpAttack.fbx"
		};


		bool CLODSample::Start()
		{
			MainCamera()->SetFarClip(1000.0f);

			SModelInitData modelInitData;

			modelInitData.animInitData.Init(
				static_cast<unsigned int>(EnAnimType::enNum), m_kAnimFilePaths);
			modelInitData.vertexBias.SetRotationXDeg(90.0f);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterAnimationBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enRegisterTextureBank);
			modelInitData.SetFlags(EnModelInitDataFlags::enCullingOff);

			modelInitData.retargetSkeltonName = "PlayerSkelton";
			modelInitData.distanceToReducingUpdate = 50.0f;


			modelInitData.modelFilePath = m_kModelFilePath[0];
			auto* baseModel = NewGO<CModelRenderer>();
			baseModel->Init(modelInitData);
			DeleteGO(baseModel);

			modelInitData.modelFilePath = m_kModelFilePath[1];
			modelInitData.SetFlags(EnModelInitDataFlags::enLoadingAsynchronous);
			modelInitData.SetFlags(EnModelInitDataFlags::enShadowCaster);
			modelInitData.lodMedelFilePath = m_kLODModelFilePath;
			modelInitData.distanceToLOD = 50.0f;

			m_modelRenderer = NewGO<CModelRenderer>();
			m_modelRenderer->SetPosition({ 0.0f ,0.0f, 0.0f });
			m_modelRenderer->SetScale(0.1f);
			m_modelRenderer->Init(modelInitData);

			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(MainCamera()->GetPosition());

			return true;
		}

		void CLODSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);
			DeleteGO(m_modelRenderer);
			return;
		}

		void CLODSample::Update(float deltaTime)
		{
			m_simpleMover->MoveCameraBySimpleMover();

			return;
		}
	}
}