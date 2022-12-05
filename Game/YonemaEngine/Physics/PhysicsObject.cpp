#include "PhysicsObject.h"
#include <PxPhysicsAPI.h>
#include "PhysicsEngine.h"
#include "PhysicsCommonFunc.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		void SMeshGeometryBuffer::RotateVertices(const nsMath::CQuaternion& rotation) noexcept
		{
			for (auto& vertices : m_vertices)
			{
				rotation.Apply(vertices);
			}

			return;
		}





		IPhysicsObject::~IPhysicsObject()
		{
			ReleaseRigidActor();
			return;
		}

		void IPhysicsObject::ReleaseRigidActor()
		{
			if (m_rigidActor == nullptr)
			{
				return;
			}

			Deactivate();

			PX_RELEASE(m_rigidActor);
			if (m_extendedData)
			{
				m_extendedData->Dead();
			}

			return;
		}

		bool IPhysicsObject::InitAsBox(
			const nsMath::CVector3& halfExtent,
			const nsMath::CVector3& position,
			EnPhysicsAttributes attribute
		)
		{
			auto res = InitCore(
				physx::PxBoxGeometry(physx::PxVec3(halfExtent.x, halfExtent.y, halfExtent.z)),
				position);

			InitExtendedData(this, attribute);

			return res;
		}

		bool IPhysicsObject::InitAsSphere(
			float radius,
			const nsMath::CVector3& position,
			EnPhysicsAttributes attribute
		)
		{
			auto res = InitCore(
				physx::PxSphereGeometry(radius),
				position);

			InitExtendedData(this, attribute);

			return res;
		}

		bool IPhysicsObject::InitAsCapsule(
			float radius,
			float halfHeight,
			const nsMath::CVector3& position,
			EnPhysicsAttributes attribute
		)
		{
			auto res = InitCore(
				physx::PxCapsuleGeometry(radius, halfHeight),
				position);

			InitExtendedData(this, attribute);

			return res;
		}



		void IPhysicsObject::InitRigidActor(physx::PxRigidActor* pRigid)
		{
			ReleaseRigidActor();

			m_rigidActor = pRigid;

			Activate();

			return;
		}


		void IPhysicsObject::SetPosition(const nsMath::CVector3& position) noexcept
		{
			if (m_rigidActor)
			{
				const auto& pose = m_rigidActor->getGlobalPose();
				m_rigidActor->setGlobalPose(physx::PxTransform(position.x, position.y, position.z, pose.q));
			}
			return;
		}

		nsMath::CVector3 IPhysicsObject::GetPosition() const noexcept
		{
			if (m_rigidActor)
			{
				const auto& pxPos = m_rigidActor->getGlobalPose().p;
				return { pxPos.x,pxPos.y,pxPos.z };
			}
			else
			{
				return nsMath::CVector3::Zero();
			}
		}

		void IPhysicsObject::SetRotation(const nsMath::CQuaternion& rotation) noexcept
		{
			if (m_rigidActor)
			{
				const auto& pose = m_rigidActor->getGlobalPose();
				m_rigidActor->setGlobalPose(
					physx::PxTransform(pose.p, 
						physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w))
				);
			}
			return;
		}

		nsMath::CQuaternion IPhysicsObject::GetRotation() const noexcept
		{
			if (m_rigidActor)
			{
				const auto& pxRot = m_rigidActor->getGlobalPose().q;
				return { pxRot.x,pxRot.y,pxRot.z,pxRot.w };
			}
			else
			{
				return nsMath::CQuaternion::Identity();
			}
		}

		void IPhysicsObject::SetPositionAndRotation(
			const nsMath::CVector3& position, const nsMath::CQuaternion& rotation)
		{
			if (m_rigidActor)
			{
				m_rigidActor->setGlobalPose(
					physx::PxTransform(position.x, position.y, position.z,
						physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w))
				);
			}
			return;
		}

		physx::PxPhysics* IPhysicsObject::GetPhysics() noexcept
		{
			return CPhysicsEngine::GetInstance()->GetPhysics();
		}

		physx::PxMaterial* IPhysicsObject::GetDefaultMaterial() noexcept
		{
			return CPhysicsEngine::GetInstance()->GetDefaultMaterial();
		}

		physx::PxCooking* IPhysicsObject::GetCooking() noexcept
		{
			return CPhysicsEngine::GetInstance()->GetCooking();
		}

		bool IPhysicsObject::UsesCCD() noexcept
		{
			return CPhysicsEngine::UsesCCD();
		}

		void IPhysicsObject::InitExtendedData(
			IPhysicsObject* physicsObject, EnPhysicsAttributes attribute)
		{
			if (m_rigidActor)
			{
				m_extendedData = new CExtendedDataForRigidActor;
				m_extendedData->Init(this, attribute);
				m_rigidActor->userData = static_cast<void*>(m_extendedData);
			}
			return;
		}


		void IPhysicsObject::Activate() noexcept
		{
			if (m_isActive)
			{
				return;
			}

			CPhysicsEngine::GetInstance()->AddActor(m_rigidActor);
			VirtualActivate();
			m_isActive = true;

			return;
		}

		void IPhysicsObject::Deactivate() noexcept
		{
			if (m_isActive == false)
			{
				return;
			}

			CPhysicsEngine::GetInstance()->RemoveActor(m_rigidActor);
			VirtualDeactivate();
			m_isActive = false;

			return;
		}


	}
}