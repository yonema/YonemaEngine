#include "NavMeshSample.h"
#include "../../YonemaEngine/AI/Navigations/NavMesh.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CNavMeshSample::Start()
		{
			// カメラ設定
			MainCamera()->SetPosition({ 0.0f, 200.0f, -200.0f });
			MainCamera()->SetTargetPosition({ 0.0f, 10.0f, 0.0f });
			MainCamera()->SetFarClip(20000.0f);
			MainCamera()->UpdateCameraParam();

			{
				EnableDebugDrawPhysicsLine();

				SLevel3DInitData initData;
				initData.mBias.MakeScaling(0.01f, 0.01f, 0.01f);
				initData.modelRootPath = "Samples";
				initData.positionBias = 0.01f;
				m_level3D.Init(
					"Assets/Level3D/Samples/testLevel.fbx",
					initData,
					[](const SLevelChipData& chipData)->bool
					{
						if (chipData.ForwardMatchName("TestPillar1"))
						{
							int a = 1;
						}
						int a = 1;
						return false;
					}
				);
			}

			// モデル生成
			{
				m_aiMoveMR = NewGO<CModelRenderer>();
				SModelInitData modelInitData;
				modelInitData.modelFilePath = "Assets/Models/Monsters/Giyara.fbx";
				modelInitData.textureRootPath = "monster";
				modelInitData.vertexBias.SetRotationXDeg(90.0f);
				m_aiMoveMR->SetPosition({ 10.0f,10.0f,0.0f });
				m_aiMoveMR->SetScale(0.1f);
				m_aiMoveMR->Init(modelInitData);

				m_targetMR = NewGO<CModelRenderer>();
				m_targetMR->SetPosition({ -10.0f,10.0f,0.0f });
				m_targetMR->SetScale(0.1f);
				m_targetMR->Init(modelInitData);
			}

			// ナビメッシュ生成
			nsMath::CMatrix mBias;
			nsMath::CQuaternion rotBias;
			rotBias.SetRotationXDeg(90.0f);
			mBias.MakeRotationFromQuaternion(rotBias);
			m_navMesh.Init("Assets/NavMeshes/Samples/navMeshTest.fbx", mBias);

			return true;
		}

		void CNavMeshSample::OnDestroy()
		{
			DeleteGO(m_targetMR);
			DeleteGO(m_aiMoveMR);

			return;
		}

		void CNavMeshSample::Update(float deltaTime)
		{
			// AIMove
			if (Keyboard()->IsTrigger(EnKeyButton::enSpace))
			{
				m_pathFinding.Execute(
					m_path,
					m_navMesh,
					m_aiMoveMR->GetPosition(),
					m_targetMR->GetPosition(),
					true,
					50.0f,
					200.0f
				);
			}

			bool isEnd = false;
			m_aiMoveMR->SetPosition(
				m_path.Move(m_aiMoveMR->GetPosition(), 100.0f, deltaTime, isEnd, false)
			);


			// TargetMove

			nsMath::CVector3 moveVec = nsMath::CVector3::Zero();
			const float moveSpeed = 100.0f * deltaTime;
			if (Keyboard()->IsPress(EnKeyButton::enW))
			{
				moveVec.z += moveSpeed;
			}
			if (Keyboard()->IsPress(EnKeyButton::enS))
			{
				moveVec.z -= moveSpeed;
			}
			if (Keyboard()->IsPress(EnKeyButton::enA))
			{
				moveVec.x -= moveSpeed;
			}
			if (Keyboard()->IsPress(EnKeyButton::enD))
			{
				moveVec.x += moveSpeed;
			}

			m_targetMR->SetPosition(m_targetMR->GetPosition() + moveVec);



			return;
		}
	}
}