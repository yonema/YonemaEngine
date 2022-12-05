#include "PhysicsDynamicObject.h"
#include <PxPhysicsAPI.h>

namespace nsYMEngine
{
	namespace nsPhysics
	{
		CPhysicsDynamicObject::~CPhysicsDynamicObject()
		{
			Terminate();
			return;
		}

		void CPhysicsDynamicObject::Terminate()
		{
			Release();
			return;
		}

		void CPhysicsDynamicObject::Release()
		{
			if (m_rigidDynamic == nullptr)
			{
				return;
			}

			ReleaseRigidActor();
			m_rigidDynamic = nullptr;

			return;
		}

		bool CPhysicsDynamicObject::InitCore(
			const physx::PxGeometry& geometry, const nsMath::CVector3& position)
		{
			Release();

			auto physics = GetPhysics();
			auto material = GetDefaultMaterial();

			m_rigidDynamic = PxCreateDynamic(
				*physics,
				physx::PxTransform(position.x, position.y, position.z),
				geometry,
				*material,
				1.0f
			);

			if (m_rigidDynamic == nullptr)
			{
				return false;
			}

			InitRigidActor(m_rigidDynamic);

			if (UsesCCD())
			{
				m_rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
			}

			return true;
		}


		void CPhysicsDynamicObject::AddForce(const nsMath::CVector3& velocity) noexcept
		{
			m_rigidDynamic->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z));
			return;
		}

		nsMath::CVector3 CPhysicsDynamicObject::GetVelocity() const noexcept
		{
			const auto& velocity = m_rigidDynamic->getLinearVelocity();
			return { velocity.x, velocity.y, velocity.z };
		}

		void CPhysicsDynamicObject::AddAngularForce(const nsMath::CVector3& angularVelocity) noexcept
		{
			m_rigidDynamic->setAngularVelocity(
				physx::PxVec3(angularVelocity.x, angularVelocity.y, angularVelocity.z));
		}

		nsMath::CVector3 CPhysicsDynamicObject::GetAngluarVelocity() const noexcept
		{
			const auto& angluarVelocity = m_rigidDynamic->getAngularVelocity();
			return { angluarVelocity.x, angluarVelocity.y, angluarVelocity.z };
		}


	}
}