#pragma once
#include "ExtendedDataForRigidActor.h"

namespace physx
{
	class PxRigidActor;
	class PxPhysics;
	class PxMaterial;
	class PxGeometry;
	class PxCooking;
}

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class IPhysicsObject : private nsUtils::SNoncopyable
		{
		protected:
			virtual bool InitCore(
				const physx::PxGeometry& geometry, const nsMath::CVector3& position) = 0 
			{
				return false;
			};

			virtual void VirtualActivate() noexcept {};

		public:
			constexpr IPhysicsObject() = default;
			virtual ~IPhysicsObject();

			/**
			 * @brief �{�b�N�X�Ƃ��ď�����
			 * @param halfExtent �{�b�N�X�̔����̕ӂ̒���
			 * @param position ���W
			 * @param attribute ��������
			 * @return �������ɐ����������H
			*/
			bool InitAsBox(
				const nsMath::CVector3& halfExtent,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			);

			/**
			 * @brief ���̃I�u�W�F�N�g�̏��L�҂��w�肵�āA�{�b�N�X�Ƃ��ď������B
			 * @tparam OwnerClass ���L�҂̌^��
			 * @param owner ���L�҂̃|�C���^
			 * @param halfExtent �{�b�N�X�̔����̕ӂ̒���
			 * @param position ���W
			 * @param attribute ��������
			 * @return �������ɐ����������H
			*/
			template <class OwnerClass>
			bool InitAsBox(
				OwnerClass* owner,
				const nsMath::CVector3& halfExtent,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			)
			{
				auto res = InitAsBox(halfExtent, position, attribute);
				SetOwner<OwnerClass>(owner);
				return res;
			}

			/**
			 * @brief �X�t�B�A�Ƃ��ď�����
			 * @param radius ���a
			 * @param position ���W
			 * @param attribute ��������
			 * @return �������ɐ����������H
			*/
			bool InitAsSphere(
				float radius,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			);

			/**
			 * @brief ���̃I�u�W�F�N�g�̏��L�҂��w�肵�āA�X�t�B�A�Ƃ��ď������B
			 * @tparam OwnerClass ���L�҂̌^��
			 * @param owner ���L�҂̃|�C���^
			 * @param radius ���a
			 * @param position ���W
			 * @param attribute ��������
			 * @return �������ɐ����������H
			*/
			template <class OwnerClass>
			bool InitAsSphere(
				OwnerClass* owner,
				float radius,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			)
			{
				auto res = InitAsSphere(radius, position, attribute);
				SetOwner<OwnerClass>(owner);
				return res;
			}

			bool InitAsCapsule(
				float radius,
				float halfHeight,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			);

			template <class OwnerClass>
			bool InitAsCapsule(
				OwnerClass* owner,
				float radius,
				float halfHeight,
				const nsMath::CVector3& position = nsMath::CVector3::Zero(),
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enNone
			)
			{
				auto res = InitAsCapsule(radius, halfHeight, position, attribute);
				SetOwner<OwnerClass>(owner);
				return res;
			}



			constexpr bool IsValid() const noexcept
			{
				return m_rigidActor ? true : false;
			}

			void SetPosition(const nsMath::CVector3& position) noexcept;

			nsMath::CVector3 GetPosition() const noexcept;

			void SetRotation(const nsMath::CQuaternion& rotation) noexcept;

			nsMath::CQuaternion GetRotation() const noexcept;

			void SetPositionAndRotation(
				const nsMath::CVector3& position, const nsMath::CQuaternion& rotation);

			const auto* GetActor() const noexcept
			{
				return m_rigidActor;
			}

			void Activate() noexcept;

			void Deactivate() noexcept;

			constexpr bool IsActive() const noexcept
			{
				return m_isActive;
			}

		protected:
			void InitRigidActor(physx::PxRigidActor* pRigid);

			void ReleaseRigidActor();

			physx::PxPhysics* GetPhysics() noexcept;

			physx::PxMaterial* GetDefaultMaterial() noexcept;

			physx::PxCooking* GetCooking() noexcept;

			bool UsesCCD() noexcept;

			void InitExtendedData(
				IPhysicsObject* physicsObject, EnPhysicsAttributes attribute);

			template <class OwnerClass>
			constexpr void SetOwner(OwnerClass* owner)
			{
				m_extendedData->SetOwner<OwnerClass>(owner);
			}

			constexpr auto* GetExtendedData() noexcept
			{
				return m_extendedData;
			}


		private:
			physx::PxRigidActor* m_rigidActor = nullptr;
			CExtendedDataForRigidActor* m_extendedData = nullptr;
			bool m_isActive = false;
		};

	}
}