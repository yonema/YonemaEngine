#pragma once
#include <PxPhysicsAPI.h>

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CContactReportCallback : public physx::PxSimulationEventCallback
		{
		public:
			constexpr CContactReportCallback() = default;
			~CContactReportCallback() = default;

		public:
			void onConstraintBreak(
				physx::PxConstraintInfo* constraints, physx::PxU32 count) override final;

			void onWake(physx::PxActor** actors, physx::PxU32 count) override final;

			void onSleep(physx::PxActor** actors, physx::PxU32 count) override final;

			void onContact(
				const physx::PxContactPairHeader& pairHeader,
				const physx::PxContactPair* pairs,
				physx::PxU32 count
			) override final;

			void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override final;

			void onAdvance(
				const physx::PxRigidBody* const* bodyBuffer,
				const physx::PxTransform* poseBuffer,
				const physx::PxU32 count
			) override final;

		private:

		};
	}
}
