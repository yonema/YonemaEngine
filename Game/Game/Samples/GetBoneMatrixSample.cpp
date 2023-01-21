#include "GetBoneMatrixSample.h"
#include "../../YonemaEngine/DebugSystem/SimpleMover.h"

namespace nsAWA
{
	namespace nsSamples
	{
		// HumanData
		const char* CGetBoneMatrixSample::m_kHumanModelFilePath = 
			"Assets/Models/player.fbx";
		const char* CGetBoneMatrixSample::
			m_kHumanAnimFilePaths[static_cast<int>(EnHumanAnimType::enNum)] = 
		{
			"Assets/Animations/Player/Sword_Idle.fbx",
		};
		const char* const CGetBoneMatrixSample::m_kHumanRightHandBoneName = "J_Bip_R_Hand";

		// MonsterData
		const char* CGetBoneMatrixSample::m_kMonsterModelFilePath = 
			"Assets/Models/Monsters/Giyara.fbx";
		const char* CGetBoneMatrixSample::
			m_kMonsterAnimFilePaths[static_cast<int>(EnMonsterAnimType::enNum)] =
		{
			"Assets/Animations/Monster/Giyara/Giyara_Idle.fbx"
		};
		const char* const CGetBoneMatrixSample::m_kMonsterRightHandBoneName = "hand_r";


		bool CGetBoneMatrixSample::Start()
		{
			EnableDebugDrawPhysicsLine();

			// InitHumanModel
			{
				SModelInitData modelInitData;
				modelInitData.modelFilePath = m_kHumanModelFilePath;
				modelInitData.animInitData.Init(
					static_cast<unsigned int>(EnHumanAnimType::enNum), m_kHumanAnimFilePaths);
				modelInitData.textureRootPath = "player";
				modelInitData.vertexBias.SetRotationXDeg(90.0f);

				m_humanMR = NewGO<CModelRenderer>();
				m_humanMR->SetPosition({ 5.0f, 4.0f, 3.0f });
				m_humanMR->SetScale(0.1f);
				m_humanMR->Init(modelInitData);
				m_humanMR->SetIsAnimationLoop(true);
				m_humanMR->SetAnimationSpeed(1.5f);
				m_humanMR->PlayAnimation(static_cast<unsigned int>(EnHumanAnimType::enDance));
			}


			// InitMonsterModel
			{
				SModelInitData modelInitData;
				modelInitData.modelFilePath = m_kMonsterModelFilePath;
				modelInitData.animInitData.Init(
					static_cast<unsigned int>(EnMonsterAnimType::enNum), m_kMonsterAnimFilePaths);
				modelInitData.textureRootPath = "monster";
				modelInitData.vertexBias.SetRotationXDeg(90.0f);

				m_monsterMR = NewGO<CModelRenderer>();
				m_monsterMR->SetPosition({ -5.0f, 4.0f, 3.0f });
				m_monsterMR->SetScale(0.05f);
				m_monsterMR->Init(modelInitData);
				m_monsterMR->SetIsAnimationLoop(true);
				m_monsterMR->SetAnimationSpeed(1.0f);
				m_monsterMR->PlayAnimation(static_cast<unsigned int>(EnMonsterAnimType::enIdle));
			}


			// GetHumanBoneMatrix
			{
				// GetBoneMatrix
				m_humanBoneId = m_humanMR->FindBoneId(m_kHumanRightHandBoneName);
				const auto& mBone = m_humanMR->GetBoneMatixWS(m_humanBoneId);

				// GetTRS
				nsMath::CVector3 bonePos;
				nsMath::CQuaternion boneRot;
				nsMath::CVector3 boneScale;
				nsMath::CalcTRSFromMatrix(bonePos, boneRot, boneScale, mBone);

				// InitTriggerBox
				constexpr float boxHalfExtent = 0.5f;
				m_humanBoneTriggerBox.InitAsBox(
					{ boxHalfExtent,boxHalfExtent * 2.0f,boxHalfExtent }, bonePos);
				m_humanBoneTriggerBox.SetRotation(boneRot);
			}

			// GetMonsterBoneMatrix
			{
				// GetBoneMatrix
				m_monsterBoneId = m_monsterMR->FindBoneId(m_kMonsterRightHandBoneName);
				const auto& mBone = m_monsterMR->GetBoneMatixWS(m_monsterBoneId);

				// GetTranslate
				nsMath::CVector3 bonePos;
				nsMath::CalcTranslateFromMatrix(bonePos, mBone);

				// InitTriggerBox
				constexpr float boxHalfExtent = 1.0f;
				m_monsterBoneTriggerBox.InitAsBox(
					{ boxHalfExtent,boxHalfExtent * 2.0f,boxHalfExtent }, bonePos);
			}


			// InitSimpleMoverForDebug
			m_simpleMover = NewGO<nsYMEngine::nsDebugSystem::CSimpleMover>();
			m_simpleMover->SetPosition(MainCamera()->GetPosition());

			return true;
		}

		void CGetBoneMatrixSample::OnDestroy()
		{
			DeleteGO(m_simpleMover);

			DeleteGO(m_monsterMR);
			DeleteGO(m_humanMR);

			return;
		}

		void CGetBoneMatrixSample::Update(float deltaTime)
		{
			// MoveCameraBySimpleMover
			MainCamera()->SetPosition(m_simpleMover->GetPosition());
			nsMath::CVector3 toTarget = m_simpleMover->GetForward();
			toTarget.Scale(5.0f);
			MainCamera()->SetTargetPosition(m_simpleMover->GetPosition() + toTarget);



			// GetHumanBoneMatrix
			{
				// GetBoneMatrix
				const auto& mBone = m_humanMR->GetBoneMatixWS(m_humanBoneId);

				// GetTRS
				nsMath::CVector3 bonePos;
				nsMath::CQuaternion boneRot;
				nsMath::CVector3 boneScale;
				nsMath::CalcTRSFromMatrix(bonePos, boneRot, boneScale, mBone);

				// SetTriggerBox
				m_humanBoneTriggerBox.SetPosition(bonePos);
				m_humanBoneTriggerBox.SetRotation(boneRot);
			}

			// GetMonsterBoneMatrix
			{
				// GetBoneMatrix
				const auto& mBone = m_monsterMR->GetBoneMatixWS(m_monsterBoneId);

				// GetTranslate
				nsMath::CVector3 bonePos;
				nsMath::CalcTranslateFromMatrix(bonePos, mBone);
				nsMath::CQuaternion boneRot;
				nsMath::CalcRotationFromMatrix(boneRot, mBone);

				// SetTriggerBox
				m_monsterBoneTriggerBox.SetPosition(bonePos);
			}

			return;
		}





	}
}