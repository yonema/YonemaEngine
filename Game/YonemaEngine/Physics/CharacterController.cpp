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
			// �e�탁���o�̃p�����[�^���Z�b�g
			m_position = position;
			m_radius = radius;
			m_halfHeight = halfHeight;

			m_myActor = m_characterCapsule.GetActor();

			physx::PxShape* shapes[1];
			m_myActor->getShapes(shapes, 1);

			m_sweepFilterCallback.SetActor(m_myActor);


			// ���[�J���|�[�Y���v�Z
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
				// �J�v�Z���R���C�_�[������
				return m_position;
			}

			UpdateGravity();

			ResetFlag();


			//// �Փˌv�Z ////

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
				// �n�ʂɕt���Ă��Ȃ�������A�d�͉��Z
				m_gravityFactor += 0.98f;
				// �d�͐���
				m_gravityFactor = std::min(m_gravityFactor, m_maxGravityFactor);
			}
			else
			{
				// �n�ʂɂ�����A�d�̓��Z�b�g�B
				m_gravityFactor = 0.0f;
			}

			return;
		}

		void CCharacterController::ResetFlag()
		{
			// �e��t���O���Z�b�g�B
			m_isOnGround = false;
			m_isOnWall = false;

			return;
		}

		nsMath::CVector3 CCharacterController::HorizontalCollisionCalculation(
			const nsMath::CVector3& nextPos, const nsMath::CVector3& originMoveDir) noexcept
		{
			auto nextPosXZ = nextPos;
			nextPosXZ.y = 0.0f;

			// �Փˌv�Z�́A1��1�̃R���W�����ł����s���Ȃ����߁A������s���B
			for (unsigned int colliCalcIdx = 0; colliCalcIdx < m_kNumCollisionCalculations; colliCalcIdx++)
			{
				bool isInitialOverlap = false;

				// nextPos����A�ړ��x�N�g�����v�Z����B
				auto toNextPosVecXZ = nextPosXZ - m_position;
				toNextPosVecXZ.y = 0.0f;
				const float moveDistXZ = toNextPosVecXZ.Length();

				auto moveDirXZ = toNextPosVecXZ;
				moveDirXZ.Normalize();

				if (moveDistXZ <= FLT_EPSILON)
				{
					// sweep��dist��0�ł����v�����ǁAdir��0���ƑS���q�b�g���Ȃ��Ȃ邽�߁A
					// �������������^���Ă����B
					moveDirXZ = nsMath::CVector3::Front();
				}

				// �Փˌ��o

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
					// �Փ˂��Ă��Ȃ����߁A�v�Z�I���B
					break;
				}

				const unsigned int kNumAnyHits = static_cast<unsigned int>(hitInfo.getNbAnyHits());

				float nearestDist = FLT_MAX;
				const physx::PxSweepHit* nearestHit = nullptr;
				// �X�C�[�v�Ńq�b�g�����I�u�W�F�N�g�̒�����A
				// ��ԋ߂��I�u�W�F�N�g���u���b�L���O�I�u�W�F�N�g�Ƃ���B
				for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
				{
					const auto& anyHit = hitInfo.getAnyHit(hitIdx);

					if (anyHit.distance < FLT_EPSILON)
					{
						// ���łɏd�Ȃ��Ă�����̂́A�X�C�[�v�Ō��m������̂́A
						// �R�[���o�b�N���d�����ĂȂ����߁AComputePenetration���g�p����B
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
						// �X�΂��Ⴂ���߁A������ɂ��ăX�L�b�v�B
						continue;
					}

					m_isOnWall = true;


					const nsMath::CVector3 hitPos(
						anyHit.position.x, anyHit.position.y, anyHit.position.z);
					const auto toHitPosVec = hitPos - m_position;

					const float distance = moveDirXZ.Dot(toHitPosVec);

					if (distance >= nearestDist)
					{
						// ���߂��I�u�W�F�N�g�����邽�߁A�X�L�b�v�B
						continue;
					}
					nearestDist = anyHit.distance;
					nearestHit = &anyHit;

				}

				if (nearestHit == nullptr)
				{
					// �Փ˂��Ă��Ȃ����߁A�v�Z�I���B
					break;
				}

				// �Փˉ���

				if (isInitialOverlap)
				{
					for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
					{
						const auto& anyHit = hitInfo.getAnyHit(hitIdx);

						// �Ȃ񂩒m��񂯂ǁAgeometry�̓����o�Ŏ������ɁA����擾����B
						physx::PxShape* myShapes[1];
						m_myActor->getShapes(myShapes, 1);
						const auto& myGeometry = myShapes[0]->getGeometry().any();
						// Sweep�ł͍ŏ�����Փ˂��Ă��锻��ł��AcomputePenetration�ł͏Փ�
						// ���Ă��Ȃ��������邽�߁A������Ƃ����ړ�������B
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
							// ���b�V�����d�Ȃ��Ă��Ȃ��A�܂��́A�d�Ȃ��Ă���[�����Ȃ��B
							// ���̃R���W�����ցB
							continue;
						}

						const nsMath::CVector3 penetrationDir(
							pxPenetrationDir.x, pxPenetrationDir.y, pxPenetrationDir.z);
						const float cosTheta = penetrationDir.Dot(nsMath::CVector3::Up());
						const float radAngle = acosf(cosTheta);
						if (radAngle < m_walkableFloorRadAngle)
						{
							// �X�΂��Ⴂ���߁A������ɂ��ăX�L�b�v�B
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
						//	//�p�ɓ��������̃L�����N�^�̐U����h�~���邽�߂ɁA
						//	//�ړ��悪�t�����ɂȂ�����ړ����L�����Z������B
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
					//	//�p�ɓ��������̃L�����N�^�̐U����h�~���邽�߂ɁA
					//	//�ړ��悪�t�����ɂȂ�����ړ����L�����Z������B
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
				// �㉺�ɓ����ĂȂ��Ƃ��́A������Ɖ��𒲂ׂ�B
				moveDirY = nsMath::CVector3::Down();
				moveDistY = 0.1f;
			}

			// �Փˌ��m

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
				// �Փ˂��Ă��Ȃ����߁A�v�Z�̕K�v�Ȃ��B
				return nextPos;
			}

			const unsigned int kNumAnyHits = static_cast<unsigned int>(hitInfo.getNbAnyHits());

			float nearestDist = FLT_MAX;
			const physx::PxSweepHit* nearestHit = nullptr;
			bool isInitialOverlap = false;
			// �X�C�[�v�Ńq�b�g�����I�u�W�F�N�g�̒�����A
			// ��ԋ߂��I�u�W�F�N�g���u���b�L���O�I�u�W�F�N�g�Ƃ���B
			for (unsigned int hitIdx = 0; hitIdx < kNumAnyHits; hitIdx++)
			{
				const auto& anyHit = hitInfo.getAnyHit(hitIdx);

				if (anyHit.distance < FLT_EPSILON)
				{
					// ���łɏd�Ȃ��Ă�����̂́A�X�C�[�v�Ō��m������̂́A
					// �R�[���o�b�N���d�����ĂȂ����߁AComputePenetration���g�p����B
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
					// �X�΂��������߁A�ǔ��肵�ăX�L�b�v�B
					continue;
				}

				if (radAngle < m_walkableFloorRadAngle)
				{
					m_isOnGround = true;
				}

				if (anyHit.distance >= nearestDist)
				{
					// ���߂��I�u�W�F�N�g�����邽�߁A�X�L�b�v�B
					continue;
				}
				nearestDist = anyHit.distance;
				nearestHit = &anyHit;

			}

			if (nearestHit == nullptr)
			{
				// �����ƏՓ˂������A�S���ǂ������̂ŁA�v�Z�̕K�v�Ȃ��B
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

				// �Ȃ񂩒m��񂯂ǁAgeometry�̓����o�Ŏ������ɁA����擾����B
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
					// ���b�V�����d�Ȃ��Ă��Ȃ�
					// ���̃R���W�����ցB
					continue;
				}

				const nsMath::CVector3 penetrationDir(
					pxPenetrationDir.x, pxPenetrationDir.y, pxPenetrationDir.z);
				const float cosTheta = penetrationDir.Dot(nsMath::CVector3::Up());
				const float radAngle = acosf(cosTheta);
				if (radAngle >= m_walkableFloorRadAngle &&
					radAngle <= nsMath::YM_PI - m_walkableFloorRadAngle)
				{
					// �X�΂��������߁A�ǔ��肵�ăX�L�b�v�B
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
				// �����ƏՓ˂������ǁA�S���ǂ���������A�v�Z�̕K�v�Ȃ��B
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