#pragma once
#include "../GameObject/GameObjectManager.h"

namespace nsYMEngine
{
	/**
	 * @brief ゲームオブジェクトの生成
	 * @tparam T 生成するゲームオブジェクトの型。nsGameObject::IGameObjectを継承している必要があります。
	 * @param[in] priority ゲームオブジェクトの優先度。低い値の優先度ほど早く更新処理が呼ばれます。
	 * @param[in] objectName ゲームオブジェクトの名前。ゲームオブジェクトの検索などで使用します。
	 * @return 生成したゲームオブジェクト
	*/
	template<class T>
	inline T* NewGO(
		EnGOPriority priority = EnGOPriority::enMid,
		const char* objectName = nullptr)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->NewGameObject<T>(priority, objectName);
	}

	/**
	 * @brief ゲームオブジェクトの破棄。この関数を呼んだ1フレーム後に破棄が行われます。
	 * @param go[in] 破棄するゲームオブジェクト
	*/
	static inline void DeleteGO(nsGameObject::IGameObject* go)
	{
		nsGameObject::CGameObjectManager::GetInstance()->DeleteGameObject(go);
	}

	/**
	 * @brief 生成されているゲームオブジェクトを、名前と型で検索(単数)。重い処理です。
	 * @tparam T 検索するゲームオブジェクトの型。nsGameObject::IGameObjectを継承している必要があります。
	 * @param[in] objectName 検索するゲームオブジェクトの名前
	 * @return 名前と型が一致する1つのゲームオブジェクト
	*/
	template<class T>
	static inline T* FindGO(const char* objectName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObject<T>(objectName);
	}

	/**
	 * @brief 生成されているゲームオブジェクトを、名前と型で検索(複数)。重い処理です。
	 * @tparam T 検索するゲームオブジェクトの型。nsGameObject::IGameObjectを継承している必要があります。
	 * @param[in] objectsName 検索するゲームオブジェクトの名前
	 * @return 名前と型が一致する全てのゲームオブジェクト
	*/
	template<class T>
	static inline  const std::vector<T*>& FindGOs(const char* objectsName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObjects<T>(objectsName);
	}

	/**
	 * @brief 生成されているゲームオブジェクトを、名前で検索(単数)。重い処理です。
	 * @param[in] objectName 検索するゲームオブジェクトの名前
	 * @return 名前が一致する1つのゲームオブジェクト
	*/
	static inline nsGameObject::IGameObject* FindGO(const char* objectName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObject(objectName);
	}

	/**
	 * @brief  生成されているゲームオブジェクトを、名前で検索(複数)。重い処理です。
	 * @param[in] objectsName 検索するゲームオブジェクトの名前
	 * @return 名前が一致する全てのゲームオブジェクト
	*/
	static inline const std::vector<nsGameObject::IGameObject*>& FindGOs(const char* objectsName)
	{
		std::function < bool(std::string* str)> func;
		sizeof(func);
		sizeof(&func);
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObjects(objectsName);
	}

	/**
	 * @brief 同名同型のゲームオブジェクト全てに対して、指定した処理を行います。重い処理です。
	 * @tparam T 処理を行いたいゲームオブジェクトの型
	 * @param[in] objectName 処理を行いたいゲームオブジェクトの名前
	 * @param[in] func 行う処理のコールバック関数。この関数でfalseを返すとQueryの処理を中断します。
	*/
	template<class T>
	static inline void QueryGOs(const char* objectName, const std::function<bool(T* go)>& func)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->
			QueryGameObjects<T>(objectName, func);
	}

}