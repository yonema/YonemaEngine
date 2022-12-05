#include "PhysicsSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"
#include <random>

namespace nsAWA
{
	namespace nsSamples
	{
		bool CPhysicsSample::Start()
		{
			MainCamera()->SetFarClip(1000.0f);

			// デバック用のPhysicsのライン描画機能の有効化。
			EnableDebugDrawPhysicsLine();
			// ラインのカリングボックスの有効化。
			SetCullingBoxForDebugDrawLine(50.0f, nsMath::CVector3::Zero());
			// ラインのカリングボックスの自動カメラフィット機能の有効化。
			EnableAutoFitCullingBoxToMainCamera();


			// プレイヤーの作成
			m_simpleMover = NewGO<nsYMEngine::nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition({ 0.0f,0.0f,0.0f });
			m_staticPlayer.InitAsSphere(5.0f, { 0.0f,0.0f,0.0f });


			// 地面となるプレーンを作成
			nsMath::CVector3 nVec = { 0.0f,1.0f,0.0f };
			nVec.Normalize();
			m_staticPlane.InitAsPlane(nVec, 0.0f);


			// 乱数生成
			constexpr float distribute = 200.0f;	// 乱数の範囲
			std::random_device rnd;     // 非決定的な乱数生成器を生成
			std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
			std::uniform_real_distribution<> rand(-distribute * 0.5f, distribute * 0.5f); // 範囲の一様乱数


			// 大量の物理オブジェクトの生成
			constexpr float shapeSize = 2.0f;
			constexpr float heightPos = 20.0f;
			for (int i = 0; i < m_kNumPhysicsObjects; i++)
			{
				m_dynamicBox[i].InitAsBox(
					{ shapeSize, shapeSize, shapeSize },
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt))}
				);
				m_dynamicSphere[i].InitAsSphere(
					shapeSize,
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt)) }
				);
				m_dynamicCapusule[i].InitAsCapsule(
					shapeSize,
					shapeSize,
					{ static_cast<float>(rand(mt)), heightPos, static_cast<float>(rand(mt)) }
				);
			}

			return true;
		}

		void CPhysicsSample::OnDestroy()
		{
			for (int i = 0; i < m_kNumPhysicsObjects; i++)
			{
				m_dynamicCapusule[i].Release();
				m_dynamicSphere[i].Release();
				m_dynamicBox[i].Release();
			}

			m_staticPlane.Release();
			return;
		}

		void CPhysicsSample::Update(float deltaTime)
		{
			m_staticPlayer.SetPositionAndRotation(
				m_simpleMover->GetPosition(),
				m_simpleMover->GetRotation()
				);


			nsMath::CVector3 toCameraVec(0.0f, 10.0f, -20.0f);
			m_simpleMover->GetRotation().Apply(toCameraVec);
			nsMath::CVector3 cameraPos = m_simpleMover->GetPosition() + toCameraVec;

			nsMath::CVector3 toTargetVec(0.0f, 0.0f, 5.0f);
			m_simpleMover->GetRotation().Apply(toTargetVec);
			nsMath::CVector3 targetPos = m_simpleMover->GetPosition() + toTargetVec;
			

			MainCamera()->SetPosition(cameraPos);
			MainCamera()->SetTargetPosition(targetPos);

			return;
		}
	}
}