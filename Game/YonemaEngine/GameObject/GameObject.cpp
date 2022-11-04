#include "GameObject.h"
namespace nsYMEngine
{
	namespace nsGameObject
	{
		const std::bitset<static_cast<int>(IGameObject::EnGameObjectFlagTable::enNumFlags)> 
			IGameObject::m_kEnableStartFlags(std::string("100"));
		const std::bitset<static_cast<int>(IGameObject::EnGameObjectFlagTable::enNumFlags)> 
			IGameObject::m_kEnableUpdateFlags(std::string("101"));

		bool IGameObject::IsMatchName(const char* objectName) const noexcept
		{
			if (strcmp(m_name.c_str(), objectName) == 0) 
			{
				return true;
			}
			return false;
		}

	}
}