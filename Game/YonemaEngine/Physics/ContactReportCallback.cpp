#include "ContactReportCallback.h"
#include "PhysicsCommonFunc.h"
#include "ExtendedDataForRigidActor.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		void CContactReportCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
		{
			//printf("onConstraintBreak\n");
			return;
		}

		void CContactReportCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
		{
			//printf("onWake\n");
			return;
		}

		void CContactReportCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
		{
			//printf("onSleep\n");
			return;
		}

		void CContactReportCallback::onContact(
			const physx::PxContactPairHeader& pairHeader,
			const physx::PxContactPair* pairs,
			physx::PxU32 count
		)
		{
			//		printf("onContact: %d pairs\n", count);

			while (count--)
			{
				const physx::PxContactPair& current = *pairs++;

				// The reported pairs can be trigger pairs or not. We only enabled contact reports for
				// trigger pairs in the filter shader, so we don't need to do further checks here. In a
				// real-world scenario you would probably need a way to tell whether one of the shapes
				// is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
				// shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
				// in a hash-set and test the reported shape pointers against it. Many options here.

				if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD))
				{
					//printf("Shape is entering trigger volume\n");
				}
				if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					//printf("Shape is leaving trigger volume\n");
				}
				if (IsTriggerShape(current.shapes[0]) && IsTriggerShape(current.shapes[1]))
				{
					//printf("Trigger-trigger overlap detected\n");
				}

			}

			return;
		}

		void CContactReportCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
		{
			//printf("onTrigger: %d trigger pairs\n", count);

			while (count--)
			{
				const physx::PxTriggerPair& current = *pairs++;
				if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					//printf("Shape is entering trigger volume\n");
					if (current.triggerActor->userData && current.otherActor->userData)
					{
						auto* triggerExtendedData = 
							static_cast<CExtendedDataForRigidActor*>(current.triggerActor->userData);
						auto* otherExtendedData =
							static_cast<CExtendedDataForRigidActor*>(current.otherActor->userData);

						triggerExtendedData->GetContactEvent()->
							m_onTriggerEnterFunc(otherExtendedData);
						otherExtendedData->GetContactEvent()->
							m_onTriggerEnterFunc(triggerExtendedData);
					}
				}
				else if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					//printf("Shape is leaving trigger volume\n");
					if (current.triggerActor->userData && current.otherActor->userData)
					{
						auto* triggerExtendedData =
							static_cast<CExtendedDataForRigidActor*>(current.triggerActor->userData);
						auto* otherExtendedData =
							static_cast<CExtendedDataForRigidActor*>(current.otherActor->userData);

						triggerExtendedData->GetContactEvent()->
							m_onTriggerExitFunc(otherExtendedData);
						otherExtendedData->GetContactEvent()->
							m_onTriggerExitFunc(triggerExtendedData);
					}
				}
				else
				{
					if (current.triggerActor->userData && current.otherActor->userData)
					{
						auto* triggerExtendedData =
							static_cast<CExtendedDataForRigidActor*>(current.triggerActor->userData);
						auto* otherExtendedData =
							static_cast<CExtendedDataForRigidActor*>(current.otherActor->userData);

						triggerExtendedData->GetContactEvent()->
							m_onTriggerStayFunc(otherExtendedData);
						otherExtendedData->GetContactEvent()->
							m_onTriggerStayFunc(triggerExtendedData);
					}
				}
			}

			return;
		}

		void CContactReportCallback::onAdvance(
			const physx::PxRigidBody* const* bodyBuffer,
			const physx::PxTransform* poseBuffer,
			const physx::PxU32 count
		)
		{
			//printf("onAdvance\n");
			return;
		}
	}
}