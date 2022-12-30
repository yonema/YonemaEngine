#pragma once
#include "../Physics/PhysicsStaticObject.h"

namespace nsYMEngine
{
	namespace nsLevel3D
	{
		struct SLevelChipData;
		struct SLevel3DinitData;
	}
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class CModelRenderer;
		}
	}
}


namespace nsYMEngine
{
	namespace nsLevel3D
	{
		class CLevelChip : private nsUtils::SNoncopyable
		{
		public:
			CLevelChip(
				const SLevelChipData& chipData,
				const SLevel3DinitData& levelInitData,
				const char* filePath
			) noexcept;
			~CLevelChip();

		private:
			nsGraphics::nsRenderers::CModelRenderer* m_modelRenderer = nullptr;
			nsPhysics::CPhysicsStaticObject m_physicsStaticObject = {};
		};

	}
}