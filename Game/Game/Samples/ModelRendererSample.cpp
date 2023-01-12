#include "ModelRendererSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"


namespace nsAWA
{
	namespace nsSamples
	{
		const char* const CModelRendererSample::m_kModelFilePath = "Assets/Models/Samples/Guild.fbx";
		//const char* const CModelRendererSample::m_kModelFilePath = "Assets/Models/Samples/multiCubeAndMatTest.fbx";

		bool CModelRendererSample::Start()
		{
			// SettingCamera
			MainCamera()->SetFarClip(10000.0f);

			// CreateModelRenderer
			SModelInitData modelInitData;
			modelInitData.modelFilePath = m_kModelFilePath;
			modelInitData.enableNodeTransform = true;
			m_modelRenderer = NewGO<CModelRenderer>();
			m_modelRenderer->Init(modelInitData);


			// CreateSimpleMover
			m_simpleMover = NewGO<nsDebugSystem::CSimpleMover>();

			return true;
		}

		void CModelRendererSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);
			DeleteGO(m_modelRenderer);

			return;
		}

		void CModelRendererSample::Update(float deltaTime)
		{



			MoveCameraBySimpleMover();

			return;
		}

		void CModelRendererSample::MoveCameraBySimpleMover() noexcept
		{
			// MoveCameraBySimpleMover
			MainCamera()->SetPosition(m_simpleMover->GetPosition());
			nsMath::CVector3 toTarget = m_simpleMover->GetForward();
			toTarget.Scale(5.0f);
			MainCamera()->SetTargetPosition(m_simpleMover->GetPosition() + toTarget);

			return;
		}

	}
}