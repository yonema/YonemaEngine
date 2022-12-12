#include "CharacterControllerSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CCharacterControllerSample::Start()
		{
			EnableDebugDrawPhysicsLine();
			SetCullingBoxForDebugDrawLine(1000.0f, nsMath::CVector3::Zero());
			MainCamera()->SetFarClip(1000.0f);

			nsPhysics::SMeshGeometryBuffer physiceMeshGeomBuffer;
			m_modelRenderer = NewGO<CModelRenderer>();

			SModelInitData modelInitData;
			modelInitData.modelFilePath = "Assets/Models/Samples/groundTest.fbx";
			modelInitData.physicsMeshGeomBuffer = &physiceMeshGeomBuffer;
			modelInitData.vertexBias.SetRotationXDeg(90.0f);

			constexpr float scaling = 10.0f;

			m_modelRenderer->SetScale(scaling);
			m_modelRenderer->SetPosition({ 0.0f, 0.0f, 0.0f });
			m_modelRenderer->Init(modelInitData);


			m_staticGround.InitAsMesh(
				physiceMeshGeomBuffer, { 0.0f,0.0f,0.0f }, scaling);

			// InitSimpleMoverForDebug
			m_simpleMover = NewGO<nsYMEngine::nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(MainCamera()->GetPosition());
			return true;
		}

		void CCharacterControllerSample::OnDestroy()
		{
			DeleteGO(m_modelRenderer);

			m_staticGround.Release();

			return;
		}

		void CCharacterControllerSample::Update(float deltaTime)
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