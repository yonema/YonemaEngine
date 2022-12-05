#include "ExtendedDataForRigidActor.h"
namespace nsYMEngine
{
	namespace nsPhysics
	{
		std::list<CExtendedDataForRigidActor*> CExtendedDataForRigidActor::m_extendedDataCollector = {};

		void CExtendedDataForRigidActor::DeleteDeadExtendedData()
		{
			m_extendedDataCollector.remove_if(
				[&](CExtendedDataForRigidActor* extendedData)
				{
					if (extendedData->IsDead())
					{
						delete extendedData;
						return true;
					}
					return false;
				});

			return;
		}

		void CExtendedDataForRigidActor::DeleteAllExtendedData()
		{
			for (auto& extendedData : m_extendedDataCollector)
			{
				delete extendedData;
			}

			m_extendedDataCollector.clear();

			return;
		}


	}
}