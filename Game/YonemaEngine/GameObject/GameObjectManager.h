#pragma once
#include "GameObject.h"

namespace nsYMEngine
{
	namespace nsGameObject
	{
		class CGameObjectManager : nsUtils::SNoncopyable
		{
		private:
			using GameObjectList = std::list<IGameObject*>;

		private:
			constexpr CGameObjectManager() = default;
			~CGameObjectManager();

		public:
			inline static CGameObjectManager* CreateInstance() noexcept
			{
				if (m_instance == nullptr)
				{
					m_instance = new CGameObjectManager();
				}
				return m_instance;
			}
			inline static void DeleteInstance() noexcept
			{
				if (m_instance)
				{
					delete m_instance;
				}
			}
			inline static CGameObjectManager* GetInstance() noexcept
			{
				return m_instance;
			}

			void Update(float deltaTime);

			constexpr const auto& GetGameObjectListArray() const noexcept
			{
				return m_gameObjectListArray;
			};

			template<class T>
			inline T* NewGameObject(EnGOPriority priority, const char* objectName);

			inline void DeleteGameObject(IGameObject* gameObject) const noexcept;

			inline void DeleteAllGameObjects() const noexcept;

			template<class T>
			T* FindGameObject(const char* objectName) const noexcept;

			IGameObject* FindGameObject(const char* objectName) noexcept;

			template<class T>
			const std::vector<T*>& FindGameObjects(const char* objectsName) noexcept;

			const std::vector<IGameObject*>& FindGameObjects(const char* objectsName) noexcept;

			template<class T>
			void QueryGameObjects(const char* objectName, std::function<bool(T* go)> func) noexcept;


		private:

			void Terminate();

			void DeleteFlaggedGameObjects();

			void ExecuteGameObjectStarts();

			void ExecuteGameObjectUpdates(float deltaTime);


		private:
			static CGameObjectManager* m_instance;
			std::array<GameObjectList, static_cast<int>(EnGOPriority::enMax)> 
				m_gameObjectListArray = {};

		};


		template<class T>
		T* CGameObjectManager::NewGameObject(EnGOPriority priority, const char* objectName)
		{
			T* newObject = new T();
			newObject->SetName(objectName);
			m_gameObjectListArray.at(static_cast<int>(priority)).emplace_back(newObject);
			return newObject;
		}

		inline void CGameObjectManager::DeleteGameObject(IGameObject* gameObject) const noexcept
		{
			if (gameObject != nullptr)
			{
				gameObject->Dead();
			}
			return;
		}

		inline void CGameObjectManager::DeleteAllGameObjects() const noexcept
		{
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					DeleteGameObject(go);
				}
			}
			return;
		}

		template<class T>
		T* CGameObjectManager::FindGameObject(const char* objectName) const noexcept
		{
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					if (go->IsMatchName(objectName) != true)
					{
						continue;
					}
					//見つけた。
					T* p = dynamic_cast<T*>(go);
					if (p == nullptr)
					{
						// 同じ名前でも、型が違えばnullptrになる。
						continue;
					}
					return p;
				}
			}
			//見つからなかった。
			return nullptr;
		}

		IGameObject* CGameObjectManager::FindGameObject(const char* objectName) noexcept
		{
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					if (go->IsMatchName(objectName) != true)
					{
						continue;
					}
					//見つけた。
					return go;
				}
			}
			//見つからなかった。
			return nullptr;
		}

		template<class T>
		const std::vector<T*>& CGameObjectManager::FindGameObjects(const char* objectsName) noexcept
		{
			static std::vector<T*> objectVectorT;
			objectVectorT.clear();
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					if (go->IsMatchName(objectsName) != true)
					{
						continue;
					}
					// 見つけた。
					T* p = dynamic_cast<T*>(go);
					// ダウンキャストできるか？
					if (p == nullptr)
					{
						continue;
					}
					objectVectorT.push_back(p);
				}
			}
			return objectVectorT;
		}

		const std::vector<IGameObject*>& CGameObjectManager::FindGameObjects(
			const char* objectsName) noexcept
		{
			static std::vector<IGameObject*> objectVector;
			objectVector.clear();
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					if (go->IsMatchName(objectsName) != true)
					{
						continue;
					}
					// 見つけた。
					objectVector.push_back(go);
				}
			}
			return objectVector;
		}

		template<class T>
		void CGameObjectManager::QueryGameObjects(const char* objectName, std::function<bool(T* go)> func) noexcept
		{
			for (auto& goList : m_gameObjectListArray)
			{
				for (auto go : goList)
				{
					if (go->IsMatchName(objectName) != true)
					{
						continue;
					}
					//見つけた。
					T* p = dynamic_cast<T*>(go);
					if (p == nullptr)
					{
						continue;
					}
					if (func(p) == false)
					{
						//クエリ中断。
						return;
					}

				}
			}
			return;
		}




	}
}