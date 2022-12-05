#pragma once
#include "PhysicsObject.h"

namespace physx
{
	class PxRigidDynamic;
	class PxGeometry;
}
namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CPhysicsDynamicObject : public IPhysicsObject
		{
		private:
			bool InitCore(
				const physx::PxGeometry& geometry,
				const nsMath::CVector3& position
			) override final;

		public:
			constexpr CPhysicsDynamicObject() = default;
			~CPhysicsDynamicObject();

			void Release();

			void AddForce(const nsMath::CVector3& velocity) noexcept;

			nsMath::CVector3 GetVelocity() const noexcept;

			void AddAngularForce(const nsMath::CVector3& AngularVelocity) noexcept;

			nsMath::CVector3 GetAngluarVelocity() const noexcept;


		private:

			void Terminate();

		private:
			physx::PxRigidDynamic* m_rigidDynamic = nullptr;
		};
	}
}
