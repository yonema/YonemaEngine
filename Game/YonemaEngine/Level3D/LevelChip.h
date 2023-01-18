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
			constexpr CLevelChip() = default;
			~CLevelChip();

			inline void AddInstance(
				const SLevelChipData& chipData, const SLevel3DinitData& levelInitData) noexcept
			{
				m_levelChipList.emplace_back(chipData);
				m_initDataList.emplace_back(levelInitData);
			}

			void Init(const char* filePath) noexcept;

		private:
			nsGraphics::nsRenderers::CModelRenderer* m_modelRenderer = nullptr;
			std::list<nsPhysics::CPhysicsStaticObject> m_physicsStaticObjectList = {};
			std::list<SLevelChipData> m_levelChipList = {};
			std::list<SLevel3DinitData> m_initDataList = {};
		};

	}
}