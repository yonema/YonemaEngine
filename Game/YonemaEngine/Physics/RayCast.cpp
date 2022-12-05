#include "RayCast.h"
#include "PhysicsEngine.h"
#include "ExtendedDataForRigidActor.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		bool RayCast(
			const nsMath::CVector3& origin,
			const nsMath::CVector3& normalDir,
			float distance,
			CExtendedDataForRigidActor* hitActorData,
			const nsMath::CVector4& color
		)
		{
			physx::PxRaycastBuffer hitCallback;
			bool isHit = CPhysicsEngine::GetInstance()->RayCast(
				origin, normalDir, distance, &hitCallback, color);

			if (isHit != true)
			{
				return isHit;
			}

			auto* hitUserData = hitCallback.getAnyHit(0).actor->userData;

			if (hitActorData && hitUserData)
			{
				;
				hitActorData = static_cast<CExtendedDataForRigidActor*>(hitUserData);
			}



		}
	}
}