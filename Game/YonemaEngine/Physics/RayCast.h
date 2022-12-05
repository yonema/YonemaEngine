#pragma once

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CExtendedDataForRigidActor;
	}
}

namespace nsYMEngine
{
	namespace nsPhysics
	{
		static bool RayCast(
			const nsMath::CVector3& origin,
			const nsMath::CVector3& normalDir,
			float distance,
			CExtendedDataForRigidActor* hitActorData,
			const nsMath::CVector4& color = nsMath::CVector4::Red()
		);
	}
}