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
				physiceMeshGeomBuffer, { 0.0f,0.0f,0.0f }, scaling, EnPhysicsAttributes::enGround);

			constexpr nsMath::CVector3 kWallSize(10.0f, 10.0f, 1.0f);
			constexpr float kWallZ = 10.0f;
			m_staticWall[0].InitAsBox(
				kWallSize, { 0.0f,kWallSize.y,kWallZ }, EnPhysicsAttributes::enWall);
			m_staticWall[1].InitAsBox(
				kWallSize,
				{ kWallSize.x - kWallSize.z, kWallSize.y, kWallZ - kWallSize.x + kWallSize.z },
				EnPhysicsAttributes::enWall
			);
			nsMath::CQuaternion rot;
			rot.SetRotationYDeg(90.0f);
			m_staticWall[1].SetRotation(rot);
			m_staticWall[2].InitAsBox(
				kWallSize,
				{ 0.0f, kWallSize.y * 2.0f - kWallSize.z, kWallZ - kWallSize.x + kWallSize.z },
				EnPhysicsAttributes::enWall
			);
			rot.SetRotationXDeg(90.0f);
			m_staticWall[2].SetRotation(rot);

			m_charaCon = NewGO<CCharacterController>();
			m_charaCon->Init<CCharacterControllerSample>(this, 1.0f, 2.0f, { 0.0f,5.0f,0.0f }, 0.5f);

			// InitSimpleMoverForDebug
			m_simpleMover = NewGO<nsYMEngine::nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetMoveSpeed(1000.0f);
			m_simpleMover->SetPosition(m_charaCon->GetPosition());
			m_prevPos = m_simpleMover->GetPosition();

			m_debugFR = NewGO<CFontRenderer>();
			SFontParameter fontParam;
			fontParam.anchor = EnAnchors::enTopLeft;
			fontParam.pivot = { 0.0f,0.0f };
			fontParam.position = { 0.0f,50.0f };
			fontParam.scale = 0.5f;
			fontParam.color = nsMath::CVector4::Black();
			m_debugFR->Init(fontParam);

			return true;
		}

		void CCharacterControllerSample::OnDestroy()
		{

			DeleteGO(m_debugFR);
			DeleteGO(m_simpleMover);

			DeleteGO(m_charaCon);
			DeleteGO(m_modelRenderer);

			m_staticWall[0].Release();
			m_staticWall[1].Release();
			m_staticWall[2].Release();
			m_staticGround.Release();

			return;
		}

		void CCharacterControllerSample::Update(float deltaTime)
		{
			physx::PxRaycastBuffer rayHitInfo;
			nsPhysics::CPhysicsEngine::GetInstance()->RayCast(
				m_charaCon->GetPosition(),
				{1.0f,0.0f,0.0f},
				10.0f,
				&rayHitInfo,
				nsMath::CVector4::Black()
				);


			nsMath::CVector3 moveVec = m_simpleMover->GetPosition() - m_prevPos;
			if (Keyboard()->IsPress(EnKeyButton::enRShift))
			{
				moveVec.Scale(100.0f);
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enSpace))
			{
				moveVec += nsMath::CVector3::Up() * 1000.0f;
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enG))
			{
				if (m_charaCon->IsEnableGravity())
				{
					m_charaCon->DisableGravity();
				}
				else
				{
					m_charaCon->EnableGravity();
				}
			}
			const auto& nextPos = m_charaCon->Execute(moveVec, deltaTime);

			m_simpleMover->SetPosition(nextPos);
			const auto velocity = nextPos - m_prevPos;
			m_prevPos = nextPos;

			// MoveTPSCameraBySimpleMover

			nsMath::CVector3 toCameraVec(0.0f, 10.0f, -20.0f);
			m_simpleMover->GetRotation().Apply(toCameraVec);
			nsMath::CVector3 cameraPos = m_simpleMover->GetPosition() + toCameraVec;

			nsMath::CVector3 toTargetVec(0.0f, 0.0f, 5.0f);
			m_simpleMover->GetRotation().Apply(toTargetVec);
			nsMath::CVector3 targetPos = m_simpleMover->GetPosition() + toTargetVec;


			MainCamera()->SetPosition(cameraPos);
			MainCamera()->SetTargetPosition(targetPos);

			constexpr unsigned int kNumWChars = 128;
			constexpr size_t kWTextSize = sizeof(wchar_t) * static_cast<size_t>(kNumWChars);
			wchar_t wText[kNumWChars] = {};
			StringCbPrintf(
				wText,
				kWTextSize,
				L"IsOnGround = %s\nIsOnWall = %s\nPosition = (%4.2f, %4.2f, %4.2f)\nVelocity = (%4.2f, %4.2f, %4.2f)\nSpeed = %4.8f",
				m_charaCon->IsOnGround() ? L"true" : L"false",
				m_charaCon->IsOnWall() ? L"true" : L"false",
				nextPos.x, nextPos.y, nextPos.z,
				velocity.x, velocity.y, velocity.z,
				velocity.Length()
				);
			m_debugFR->SetText(wText);

			return;
		}
	}
}