#include "CharacterControllerSample.h"
namespace nsAWA
{
	namespace nsSamples
	{
		bool CCharacterControllerSample::Start()
		{
			EnableDebugDrawPhysicsLine();
			SetCullingBoxForDebugDrawLine(100.0f, nsMath::CVector3::Zero());

			nsPhysics::SMeshGeometryBuffer physiceMeshGeomBuffer;
			m_modelRenderer = NewGO<CModelRenderer>();

			SModelInitData modelInitData;
			modelInitData.modelFilePath = "Assets/Models/groundTest.fbx";
			modelInitData.physicsMeshGeomBuffer = &physiceMeshGeomBuffer;
			modelInitData.vertexBias.SetRotationXDeg(90.0f);

			constexpr float scaling = 10.0f;

			m_modelRenderer->SetScale(scaling);
			m_modelRenderer->SetPosition({ 0.0f, 0.0f, 0.0f });
			m_modelRenderer->Init(modelInitData);


			m_staticGround.InitAsMesh(
				physiceMeshGeomBuffer, { 0.0f,0.0f,0.0f }, scaling);


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

			return;
		}
	}
}