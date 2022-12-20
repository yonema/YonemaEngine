#include "CharacterController.h"
#include "PhysicsEngine.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		const float CCharacterController::m_kPenetrationDepthThreshold = 0.0001f;
		const float CCharacterController::m_kDefaultMaxGravityFactor = 10000.0f;


		CCharacterController::CCharacterController()
		{

			return;
		}

		void CCharacterController::OnDestroy()
		{
			Terminate();
			return;
		}

		void CCharacterController::Terminate()
		{
			Release();

			return;
		}

		void CCharacterController::Release()
		{
			m_myActor = nullptr;
			m_characterCapsule.Release();

			return;
		}

		bool CCharacterController::Init(
			float radius,
			float halfHeight,
			const nsMath::CVector3& position,
			float heightPivot,
			EnPhysicsAttributes attribute
		)
		{
			bool res = m_characterCapsule.InitAsCapsule(
				radius, halfHeight, position, attribute);

			if (res != true)
			{
				return false;
			}

			InitCore(radius, halfHeight, position, heightPivot);

			return true;
		}

		void CCharacterController::InitCore(
			float radius,
			float halfHeight,
			const nsMath::CVector3& position,
			float heightPivot
		)
		{
			// 各種メンバのパラメータをセット
			m_position = position;
			m_radius = radius;
			m_halfHeight = halfHeight;

			m_myActor = m_characterCapsule.GetActor();

			physx::PxShape* shapes[1];
			m_myActor->getShapes(shapes, 1);

			m_sweepFilterCallback.SetActor(m_myActor);


			// ローカルポーズを計算
			const float kTotalHalfHeight = halfHeight + radius;
			float relativeHeight = nsMath::Lerpf(heightPivot, kTotalHalfHeight, -kTotalHalfHeight);

			physx::PxTransform relativePose(
				physx::PxVec3(0.0f, relativeHeight, 0.0f),
				physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1))
			);

			shapes[0]->setLocalPose(relativePose);

			return;
		}

		const nsMath::CVector3& CCharacterController::Execute(
			const nsMath::CVector3& moveVec, float deltaTime)noexcept
		{
			if (m_characterCapsule.IsValid() != true)
			{
				// カプセルコライダーが無効
				return m_position;
			}

			UpdateGravity();

			ResetFlag();


			//// 衝突計算 ////

			const auto moveVecWithDeltaTime =
				(moveVec + 
					(nsMath::CVector3::Down() * m_gravityFactor * 0.2f * m_gravityScale)
					) * deltaTime;

			auto nextPos = m_position + moveVecWithDeltaTime;

			auto originMoveDir = moveVecWithDeltaTime;
			originMoveDir.Normalize();

			const auto nextPosXZ = HorizontalCollisionCalculation(nextPos, originMoveDir);

			nextPos.x = nextPosXZ.x;
			nextPos.z = nextPosXZ.z;

			m_position.x = nextPosXZ.x;
			m_position.z = nextPosXZ.z;
			m_characterCapsule.SetPosition(m_position);


			nextPos = VerticalCollisionCalculation(nextPos);

			m_position = nextPos;
			m_characterCapsule.SetPosition(m_position);
			return m_position;




		
		}


		void CCharacterController::UpdateGravity()
		{
			if (m_enableGravity && m_isOnGround != true)
			{
				// 地面に付いていなかったら、重力加算
				m_gravityFactor += 0.98f;
				// 重力制限
				m_gravityFactor = std::min(m_gravityFactor, m_maxGravityFactor);
			}
			else
			{
				// 地面についたら、重力リセット。
				m_gravityFactor = 0.0f;
			}

			return;
		}

		void CCharacterController::ResetFlag()
		{
			// 各種フラグリセット。
			m_isOnGround = false;
			m_isOnWall = false;

			return;
		}

		nsMath::CVector3 CCharacterController::HorizontalCollisionCalculation(
			const nsMath::CVector3& nextPos, const nsMath::CVector3& originMoveDir) noexcept
		{
			auto nextPosXZ = nextPos;
			nextPosXZ.y = 0.0f;

			// 衝突計算は、1対1のコリジョンでしか行えないため、複数回行う。
			for (unsigned int colliCalcIdx = 0; colliCalcIdx < m_kNumCollisionCalculations; colliCalcIdx++)
			{
				bool isInitialOverlap = false;

				// nextPosから、移動ベクトルを計算する。
				auto toNextPosVecXZ = nextPosXZ - m_position;
				toNextPosVecXZ.y = 0.0f;
				const float moveDistXZ = toNextPosVecXZ.Length();

				auto moveDirXZ = toNextPosVecXZ;
				moveDirXZ.Normalize();

				if (moveDistXZ <= FLT_EPSILON)
				{
					// sweepのdistは0でも大丈夫だけど、dirは0だと全部ヒットしなくなるため、
					// 何かしら方向を与えておく。
					moveDirXZ = nsMath::CVector3::Front();
				}

				// 衝突検出

				physx::PxSweepHit hitBuffers[m_kHitBufferSize];
				physx::PxSweepBuffer hitInfo(hitBuffers, m_kHitBufferSize);

				bool isHit = CPhysicsEngine::GetInstance()->SweepMultiple(
					m_characterCapsule,
					moveDirXZ,
					moveDistXZ,
					&hitInfo,
					&m_sweepFilterCallback
				);

				if (isHit != true)
				{
					// 衝突していないため、計算終了。
					break;
				}

				const unsigned int kNumAnyHits = static_cast<unsigned int>(hitInfo.getNbAnyHits());

				float nearestDist = FLT_MAX;
				const physx::PxSweepHit* nearestHit = nullptr;
				// スイープでヒットしたオブジェクトの中から、
				// 一番近いオブジェクトをブロッキングオブジェクトとする。
				for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
				{
					const auto& anyHit = hitInfo.getAnyHit(hitIdx);

					if (anyHit.distance < FLT_EPSILON)
					{
						// すでに重なっているものは、スイープで検知するものの、
						// コールバックが仕事してないため、ComputePenetrationを使用する。
						isInitialOverlap = true;
						nearestHit = &anyHit;
						break;
					}

					const nsMath::CVector3 hitNormal(
						anyHit.normal.x, anyHit.normal.y, anyHit.normal.z);
					const float cosTheta = hitNormal.Dot(nsMath::CVector3::Up());
					const float radAngle = acosf(cosTheta);
					if (radAngle < m_walkableFloorRadAngle)
					{
						// 傾斜が低いため、床判定にしてスキップ。
						continue;
					}

					m_isOnWall = true;


					const nsMath::CVector3 hitPos(
						anyHit.position.x, anyHit.position.y, anyHit.position.z);
					const auto toHitPosVec = hitPos - m_position;

					const float distance = moveDirXZ.Dot(toHitPosVec);

					if (distance >= nearestDist)
					{
						// より近いオブジェクトがあるため、スキップ。
						continue;
					}
					nearestDist = anyHit.distance;
					nearestHit = &anyHit;

				}

				if (nearestHit == nullptr)
				{
					// 衝突していないため、計算終了。
					break;
				}

				// 衝突解決

				if (isInitialOverlap)
				{
					for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
					{
						const auto& anyHit = hitInfo.getAnyHit(hitIdx);

						// なんか知らんけど、geometryはメンバで持たずに、毎回取得する。
						physx::PxShape* myShapes[1];
						m_myActor->getShapes(myShapes, 1);
						const auto& myGeometry = myShapes[0]->getGeometry().any();
						// Sweepでは最初から衝突している判定でも、computePenetrationでは衝突
						// していない時があるため、ちょっとだけ移動させる。
						const float offsetF = std::min(m_radius, 0.1f);
						const auto offsetV = moveDirXZ * offsetF;
						const auto glPos = m_position + offsetV;
						physx::PxTransform myGlobalPose(glPos.x, glPos.y, glPos.z, m_myActor->getGlobalPose().q);
						const physx::PxTransform myFinalPose = myGlobalPose * myShapes[0]->getLocalPose();

						const auto& hitActor = anyHit.actor;
						physx::PxShape* hitShapes[1];
						hitActor->getShapes(hitShapes, 1);
						const auto& hitGeometry = hitShapes[0]->getGeometry().any();
						const auto& hitFinalPose = physx::PxShapeExt::getGlobalPose(*hitShapes[0], *hitActor);

						physx::PxVec3 pxPenetrationDir = {};
						physx::PxF32 pxPenetrationDepth = 0.0f;

						bool isPenetration = physx::PxGeometryQuery::computePenetration(
							pxPenetrationDir,
							pxPenetrationDepth,
							myGeometry,
							myFinalPose,
							hitGeometry,
							hitFinalPose
						);

						if (isPenetration != true)
						{
							// メッシュが重なっていない、または、重なっている深さがない。
							// 次のコリジョンへ。
							continue;
						}

						const nsMath::CVector3 penetrationDir(
							pxPenetrationDir.x, pxPenetrationDir.y, pxPenetrationDir.z);
						const float cosTheta = penetrationDir.Dot(nsMath::CVector3::Up());
						const float radAngle = acosf(cosTheta);
						if (radAngle < m_walkableFloorRadAngle)
						{
							// 傾斜が低いため、床判定にしてスキップ。
							continue;
						}

						m_isOnWall = true;

						auto panetrationDirXZ = penetrationDir;
						panetrationDirXZ.y = 0.0f;
						panetrationDirXZ.Normalize();
						auto penetrationVec = m_position - nextPosXZ;
						//penetrationVec += offsetV;
						penetrationVec.y = 0.0f;
						pxPenetrationDepth -= offsetF;
						
						if (pxPenetrationDepth >= penetrationVec.Length())
						{
							penetrationVec = penetrationDir * pxPenetrationDepth;
						}


						const float detectionDistXZ = panetrationDirXZ.Dot(penetrationVec);
						if (detectionDistXZ < FLT_EPSILON)
						{
							continue;
						}
						const auto detectionVecXZ = panetrationDirXZ * detectionDistXZ;
						if (detectionVecXZ.LengthSq() < FLT_EPSILON)
						{
							continue;
						}

						nextPosXZ += detectionVecXZ;

						//nsMath::CVector3 currentDir;
						//currentDir.Subtract(nextPosXZ, m_position);
						//currentDir.y = 0.0f;
						//currentDir.Normalize();
						//if (currentDir.Dot(originMoveDir) < 0.0f) {
						//	//角に入った時のキャラクタの振動を防止するために、
						//	//移動先が逆向きになったら移動をキャンセルする。
						//	nextPosXZ.x = m_position.x;
						//	nextPosXZ.z = m_position.z;
						//	break;
						//}
					}
				}
				else
				{
					nsMath::CVector3 hitPosXZ = m_position + moveDirXZ * nearestHit->distance;
					hitPosXZ.y = 0.0f;
					nsMath::CVector3 hitNormalXZ(nearestHit->normal.x, 0.0f, nearestHit->normal.z);
					hitNormalXZ.Normalize();

					const auto penetrationVecXZ = hitPosXZ - nextPosXZ;
					const float detectionDistXZ = hitNormalXZ.Dot(penetrationVecXZ);
					const auto detectionVecXZ = hitNormalXZ * detectionDistXZ;
					if (detectionVecXZ.LengthSq() < FLT_EPSILON)
					{
						break;
					}

					nextPosXZ += detectionVecXZ;

					//nsMath::CVector3 currentDir;
					//currentDir.Subtract(nextPosXZ, m_position);
					//currentDir.y = 0.0f;
					//currentDir.Normalize();
					//if (currentDir.Dot(originMoveDir) < 0.0f) {
					//	//角に入った時のキャラクタの振動を防止するために、
					//	//移動先が逆向きになったら移動をキャンセルする。
					//	nextPosXZ.x = m_position.x;
					//	nextPosXZ.z = m_position.z;
					//	break;
					//}
				}


			}


			return nextPosXZ;
		}



		nsMath::CVector3 CCharacterController::VerticalCollisionCalculation(const nsMath::CVector3& nextPos) noexcept
		{
			const auto toNextPosVecY = nextPos - m_position;
			auto moveDirY = toNextPosVecY;
			moveDirY.Normalize();
			float moveDistY = toNextPosVecY.Length();
			if (moveDistY <= FLT_EPSILON)
			{
				// 上下に動いてないときは、ちょっと下を調べる。
				moveDirY = nsMath::CVector3::Down();
				moveDistY = 0.1f;
			}

			// 衝突検知

			physx::PxSweepHit hitBuffers[m_kHitBufferSize];
			physx::PxSweepBuffer hitInfo(hitBuffers, m_kHitBufferSize);

			bool isHit = CPhysicsEngine::GetInstance()->SweepMultiple(
				m_characterCapsule,
				moveDirY,
				moveDistY,
				&hitInfo,
				&m_sweepFilterCallback
			);

			if (isHit != true)
			{
				// 衝突していないため、計算の必要なし。
				return nextPos;
			}

			const unsigned int kNumAnyHits = static_cast<unsigned int>(hitInfo.getNbAnyHits());

			float nearestDist = FLT_MAX;
			const physx::PxSweepHit* nearestHit = nullptr;
			bool isInitialOverlap = false;
			// スイープでヒットしたオブジェクトの中から、
			// 一番近いオブジェクトをブロッキングオブジェクトとする。
			for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
			{
				const auto& anyHit = hitInfo.getAnyHit(hitIdx);

				if (anyHit.distance < FLT_EPSILON)
				{
					// すでに重なっているものは、スイープで検知するものの、
					// コールバックが仕事してないため、ComputePenetrationを使用する。
					isInitialOverlap = true;
					nearestHit = &anyHit;
					break;
				}

				const nsMath::CVector3 hitNormal(
					anyHit.normal.x, anyHit.normal.y, anyHit.normal.z);
				const float cosTheta = hitNormal.Dot(nsMath::CVector3::Up());
				const float radAngle = acosf(cosTheta);
				if (radAngle >= m_walkableFloorRadAngle &&
					radAngle <= nsMath::YM_PI - m_walkableFloorRadAngle)
				{
					// 傾斜が高いため、壁判定してスキップ。
					continue;
				}

				if (radAngle < m_walkableFloorRadAngle)
				{
					m_isOnGround = true;
				}

				if (anyHit.distance >= nearestDist)
				{
					// より近いオブジェクトがあるため、スキップ。
					continue;
				}
				nearestDist = anyHit.distance;
				nearestHit = &anyHit;

			}

			if (nearestHit == nullptr)
			{
				// 何かと衝突したが、全部壁だったので、計算の必要なし。
				return nextPos;
			}

			if (isInitialOverlap != true)
			{
				return m_position + moveDirY * nearestHit->distance;
			}

			nearestDist = FLT_MAX;
			nsMath::CVector3 nearestDir = nsMath::CVector3::Up();
			const float offsetF = std::min(m_halfHeight, 0.1f);
			const auto offsetV = moveDirY * offsetF;
			for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
			{
				const auto& anyHit = hitInfo.getAnyHit(hitIdx);

				// なんか知らんけど、geometryはメンバで持たずに、毎回取得する。
				physx::PxShape* myShapes[1];
				m_myActor->getShapes(myShapes, 1);
				const auto& myGeometry = myShapes[0]->getGeometry().any();
				//const auto& myFinalPose = physx::PxShapeExt::getGlobalPose(*myShapes[0], *m_myActor);

				const auto glPos = m_position + offsetV;
				physx::PxTransform myGlobalPose(glPos.x, glPos.y, glPos.z, m_myActor->getGlobalPose().q);
				const physx::PxTransform myFinalPose = myGlobalPose * myShapes[0]->getLocalPose();

				const auto& hitActor = anyHit.actor;
				physx::PxShape* hitShapes[1];
				hitActor->getShapes(hitShapes, 1);
				const auto& hitGeometry = hitShapes[0]->getGeometry().any();
				const auto& hitFinalPose = physx::PxShapeExt::getGlobalPose(*hitShapes[0], *hitActor);

				physx::PxVec3 pxPenetrationDir = {};
				physx::PxF32 pxPenetrationDepth = 0.0f;

				bool isPenetration = physx::PxGeometryQuery::computePenetration(
					pxPenetrationDir,
					pxPenetrationDepth,
					myGeometry,
					myFinalPose,
					hitGeometry,
					hitFinalPose
				);

				if (isPenetration != true)
				{
					// メッシュが重なっていない
					// 次のコリジョンへ。
					continue;
				}

				const nsMath::CVector3 penetrationDir(
					pxPenetrationDir.x, pxPenetrationDir.y, pxPenetrationDir.z);
				const float cosTheta = penetrationDir.Dot(nsMath::CVector3::Up());
				const float radAngle = acosf(cosTheta);
				if (radAngle >= m_walkableFloorRadAngle &&
					radAngle <= nsMath::YM_PI - m_walkableFloorRadAngle)
				{
					// 傾斜が高いため、壁判定してスキップ。
					continue;
				}
				if (radAngle < m_walkableFloorRadAngle)
				{
					m_isOnGround = true;
				}

				const auto panetrationVec = penetrationDir * pxPenetrationDepth;
				float penetrationDist = nsMath::CVector3::Up().Dot(panetrationVec);

				if (penetrationDist >= nearestDist)
				{
					continue;
				}

				nearestDist = penetrationDist;
				nearestDir = penetrationDir;

			}


			if (nearestDist >= (FLT_MAX - FLT_EPSILON))
			{
				// 何かと衝突したけど、全部壁だったから、計算の必要なし。
				return nextPos;
			}

			nsMath::CVector3 detectionDir(0.0f, nearestDir.y, 0.0f);
			detectionDir.Normalize();
			const float detectionDist = detectionDir.Dot(nearestDir * nearestDist);

			return m_position +
				detectionDir * std::max(0.0f, detectionDist - offsetF);
		}




	}
}