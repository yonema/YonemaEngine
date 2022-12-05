#pragma once
#include <PxPhysicsAPI.h>

namespace nsYMEngine
{
	namespace nsPhysics
	{

#define PX_RELEASE(x) if(x) { x->release(); x = NULL; }

		inline bool IsTriggerShape(physx::PxShape* shape) noexcept
		{
			// Detects native built-in triggers.
			if (shape->getFlags() & physx::PxShapeFlag::eTRIGGER_SHAPE)
				return true;

			return false;
		}

		inline bool IsTrigger(const physx::PxFilterData& data)
		{
			if (data.word0 != 0xffffffff)
				return false;
			if (data.word1 != 0xffffffff)
				return false;
			if (data.word2 != 0xffffffff)
				return false;
			if (data.word3 != 0xffffffff)
				return false;
			return true;
		}

		constexpr nsMath::CVector3 ConverteFromPxDebugColorToMyVec3(physx::PxU32 srcColor) noexcept
		{
			switch (srcColor)
			{
			case physx::PxDebugColor::eARGB_BLACK:
				return { 0.0f, 0.0f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_RED:
				return { 1.0f, 0.0f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_GREEN:
				return { 0.0f, 1.0f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_BLUE:
				return { 0.0f, 0.0f, 1.0f };
				break;
			case physx::PxDebugColor::eARGB_YELLOW:
				return { 1.0f, 1.0f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_MAGENTA:
				return { 1.0f, 0.0f, 1.0f };
				break;
			case physx::PxDebugColor::eARGB_CYAN:
				return { 0.0f, 1.0f, 1.0f };
				break;
			case physx::PxDebugColor::eARGB_WHITE:
				return { 1.0f, 1.0f, 1.0f };
				break;
			case physx::PxDebugColor::eARGB_GREY:
				return { 0.5f, 0.5f, 0.5f };
				break;
			case physx::PxDebugColor::eARGB_DARKRED:
				return { 0.54f, 0.0f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_DARKGREEN:
				return { 0.0f, 0.39f, 0.0f };
				break;
			case physx::PxDebugColor::eARGB_DARKBLUE:
				return { 0.0f, 0.0f, 0.54f };
				break;
			}

			// デフォルトは赤色
			return { 1.0f, 0.0f, 0.0f };
		}
	}
}