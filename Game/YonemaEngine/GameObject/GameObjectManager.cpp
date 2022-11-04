#include "GameObjectManager.h"
namespace nsYMEngine
{
	namespace nsGameObject
	{
		CGameObjectManager* CGameObjectManager::m_instance = nullptr;

		CGameObjectManager::~CGameObjectManager()
		{
			Terminate();
			m_instance = nullptr;
			return;
		}

		void CGameObjectManager::Terminate()
		{
			DeleteAllGameObjects();
			DeleteFlaggedGameObjects();
			return;
		}

		void CGameObjectManager::Update(float deltaTime)
		{
			DeleteFlaggedGameObjects();

			ExecuteGameObjectStarts();

			ExecuteGameObjectUpdates(deltaTime);

			return;
		}

		void CGameObjectManager::DeleteFlaggedGameObjects()
		{
			for (auto& goList : m_gameObjectListArray) 
			{
				goList.remove_if(
					[&](IGameObject* go)
					{
						if (go->IsDead())
						{
							go->OnDestroyWrapper();
							delete go;
							return true;
						}
						return false;
					}
				);
			}

			return;
		}

		void CGameObjectManager::ExecuteGameObjectStarts()
		{
			for (auto& goList : m_gameObjectListArray) 
			{
				for (auto& go : goList)
				{
					go->StartWrapper();
				}
			}
			return;
		}

		void CGameObjectManager::ExecuteGameObjectUpdates(float deltaTime)
		{
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto& go : goList) 
				{
					go->UpdateWrapper(deltaTime);
				}
			}
			return;
		}

	}
}