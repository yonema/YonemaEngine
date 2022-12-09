#pragma once
#include "PhysicsObject.h"
namespace physx
{
	class PxRigidStatic;
	class PxGeometry;
}

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CPhysicsTriggerObject : public IPhysicsObject
		{
		private:
			bool InitCore(
				const physx::PxGeometry& geometry,
				const nsMath::CVector3& position
			) override final;

			void VirtualActivate() noexcept override final;

		public:
			constexpr CPhysicsTriggerObject() = default;
			~CPhysicsTriggerObject();

			void Release();

			void UpdateContactEvent();

			/**
			 * @brief 他の物理オブジェクトが、自分のトリガーに入ってきた瞬間に呼ばれるコールバック関数を設定。
			 * @param func コールバック関数
			*/
			void SetOnTriggerEnterFunc(const SContactEvent::ContactEventFunc& func) noexcept;

			/**
			 * @brief 他の物理オブジェクトが、自分のトリガーから出て行った瞬間に呼ばれるコールバック関数を設定。
			 * @param func コールバック関数
			*/
			void SetOnTriggerExitFunc(const SContactEvent::ContactEventFunc& func) noexcept;

			/**
			 * @brief 他の物理オブジェクトが、自分のトリガーに入っている間、呼ばれ続けるコールバック関数を設定。
			 * @param func コールバック関数
			*/
			void SetOnTriggerStayFunc(const SContactEvent::ContactEventFunc& func) noexcept;

		private:

			void Terminate();

		private:
			physx::PxRigidStatic* m_rigidTrigger = nullptr;

			/**
			 * @brief 接触中のExtendedDataを保持するマップ
			 * <CExtendedDataForRigidActor*, bool>
			 * @param CExtendedDataForRigidActor 接触中のExtendedData
			 * @param bool 現在のフレームでも接触中だったか？
			*/
			std::unordered_map<CExtendedDataForRigidActor*, bool> m_extendedDataMapInContact = {};
		};

	}
}