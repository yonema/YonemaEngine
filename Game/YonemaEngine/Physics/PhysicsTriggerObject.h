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
			 * @brief ���̕����I�u�W�F�N�g���A�����̃g���K�[�ɓ����Ă����u�ԂɌĂ΂��R�[���o�b�N�֐���ݒ�B
			 * @param func �R�[���o�b�N�֐�
			*/
			void SetOnTriggerEnterFunc(const SContactEvent::ContactEventFunc& func) noexcept;

			/**
			 * @brief ���̕����I�u�W�F�N�g���A�����̃g���K�[����o�čs�����u�ԂɌĂ΂��R�[���o�b�N�֐���ݒ�B
			 * @param func �R�[���o�b�N�֐�
			*/
			void SetOnTriggerExitFunc(const SContactEvent::ContactEventFunc& func) noexcept;

			/**
			 * @brief ���̕����I�u�W�F�N�g���A�����̃g���K�[�ɓ����Ă���ԁA�Ă΂ꑱ����R�[���o�b�N�֐���ݒ�B
			 * @param func �R�[���o�b�N�֐�
			*/
			void SetOnTriggerStayFunc(const SContactEvent::ContactEventFunc& func) noexcept;

		private:

			void Terminate();

		private:
			physx::PxRigidStatic* m_rigidTrigger = nullptr;

			/**
			 * @brief �ڐG����ExtendedData��ێ�����}�b�v
			 * <CExtendedDataForRigidActor*, bool>
			 * @param CExtendedDataForRigidActor �ڐG����ExtendedData
			 * @param bool ���݂̃t���[���ł��ڐG�����������H
			*/
			std::unordered_map<CExtendedDataForRigidActor*, bool> m_extendedDataMapInContact = {};
		};

	}
}