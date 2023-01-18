#include "DrawWorldSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CDrawWorldSample::Start()
		{
			EnableDebugDrawPhysicsLine();

			// CameraSetting
			MainCamera()->SetFarClip(10000.0f);

			// CreateWorldFromLevel3D
			SLevel3DInitData initData;
			//initData.mBias.MakeScaling(0.01f, 0.01f, 0.01f);
			initData.modelRootPath = "Samples";
			//initData.positionBias = 0.01f;
			initData.isCreateStaticPhysicsObjectForAll = false;
			m_level3D.Init(
				"Assets/Level3D/Samples/WorldLevelTest.fbx",
				initData,
				[](const SLevelChipData& chipData)->bool
				{
					return false;
				}
			);


			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition({ 0.0f,10.0f,0.0f });

			return true;
		}

		void CDrawWorldSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);
			return;
		}

		void CDrawWorldSample::Update(float deltaTime)
		{
			m_simpleMover->MoveCameraBySimpleMover();

			return;
		}
	}
}