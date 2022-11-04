#pragma once
#include "../GameObject/GameObjectManager.h"

namespace nsYMEngine
{
	/**
	 * @brief �Q�[���I�u�W�F�N�g�̐���
	 * @tparam T ��������Q�[���I�u�W�F�N�g�̌^�BnsGameObject::IGameObject���p�����Ă���K�v������܂��B
	 * @param[in] priority �Q�[���I�u�W�F�N�g�̗D��x�B�Ⴂ�l�̗D��x�قǑ����X�V�������Ă΂�܂��B
	 * @param[in] objectName �Q�[���I�u�W�F�N�g�̖��O�B�Q�[���I�u�W�F�N�g�̌����ȂǂŎg�p���܂��B
	 * @return ���������Q�[���I�u�W�F�N�g
	*/
	template<class T>
	inline T* NewGO(
		EnGOPriority priority = EnGOPriority::enMid,
		const char* objectName = nullptr)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->NewGameObject<T>(priority, objectName);
	}

	/**
	 * @brief �Q�[���I�u�W�F�N�g�̔j���B���̊֐����Ă�1�t���[����ɔj�����s���܂��B
	 * @param go[in] �j������Q�[���I�u�W�F�N�g
	*/
	static inline void DeleteGO(nsGameObject::IGameObject* go)
	{
		nsGameObject::CGameObjectManager::GetInstance()->DeleteGameObject(go);
	}

	/**
	 * @brief ��������Ă���Q�[���I�u�W�F�N�g���A���O�ƌ^�Ō���(�P��)�B�d�������ł��B
	 * @tparam T ��������Q�[���I�u�W�F�N�g�̌^�BnsGameObject::IGameObject���p�����Ă���K�v������܂��B
	 * @param[in] objectName ��������Q�[���I�u�W�F�N�g�̖��O
	 * @return ���O�ƌ^����v����1�̃Q�[���I�u�W�F�N�g
	*/
	template<class T>
	static inline T* FindGO(const char* objectName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObject<T>(objectName);
	}

	/**
	 * @brief ��������Ă���Q�[���I�u�W�F�N�g���A���O�ƌ^�Ō���(����)�B�d�������ł��B
	 * @tparam T ��������Q�[���I�u�W�F�N�g�̌^�BnsGameObject::IGameObject���p�����Ă���K�v������܂��B
	 * @param[in] objectsName ��������Q�[���I�u�W�F�N�g�̖��O
	 * @return ���O�ƌ^����v����S�ẴQ�[���I�u�W�F�N�g
	*/
	template<class T>
	static inline  const std::vector<T*>& FindGOs(const char* objectsName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObjects<T>(objectsName);
	}

	/**
	 * @brief ��������Ă���Q�[���I�u�W�F�N�g���A���O�Ō���(�P��)�B�d�������ł��B
	 * @param[in] objectName ��������Q�[���I�u�W�F�N�g�̖��O
	 * @return ���O����v����1�̃Q�[���I�u�W�F�N�g
	*/
	static inline nsGameObject::IGameObject* FindGO(const char* objectName)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObject(objectName);
	}

	/**
	 * @brief  ��������Ă���Q�[���I�u�W�F�N�g���A���O�Ō���(����)�B�d�������ł��B
	 * @param[in] objectsName ��������Q�[���I�u�W�F�N�g�̖��O
	 * @return ���O����v����S�ẴQ�[���I�u�W�F�N�g
	*/
	static inline const std::vector<nsGameObject::IGameObject*>& FindGOs(const char* objectsName)
	{
		std::function < bool(std::string* str)> func;
		sizeof(func);
		sizeof(&func);
		return nsGameObject::CGameObjectManager::GetInstance()->FindGameObjects(objectsName);
	}

	/**
	 * @brief �������^�̃Q�[���I�u�W�F�N�g�S�Ăɑ΂��āA�w�肵���������s���܂��B�d�������ł��B
	 * @tparam T �������s�������Q�[���I�u�W�F�N�g�̌^
	 * @param[in] objectName �������s�������Q�[���I�u�W�F�N�g�̖��O
	 * @param[in] func �s�������̃R�[���o�b�N�֐��B���̊֐���false��Ԃ���Query�̏����𒆒f���܂��B
	*/
	template<class T>
	static inline void QueryGOs(const char* objectName, const std::function<bool(T* go)>& func)
	{
		return nsGameObject::CGameObjectManager::GetInstance()->
			QueryGameObjects<T>(objectName, func);
	}

}