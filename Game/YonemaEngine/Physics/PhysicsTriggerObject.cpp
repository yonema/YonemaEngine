#include "PhysicsTriggerObject.h"
#include <PxPhysicsAPI.h>
#include "PhysicsEngine.h"

namespace nsYMEngine
{
	namespace nsPhysics
	{
		bool CPhysicsTriggerObject::InitCore(
			const physx::PxGeometry& geometry,
			const nsMath::CVector3& position
		)
		{
			Release();

			auto physics = GetPhysics();
			auto material = GetDefaultMaterial();

			m_rigidTrigger = PxCreateStatic(
				*physics,
				physx::PxTransform(position.x, position.y, position.z),
				geometry,
				*material
			);

			// �V�F�C�v�̃t���O���g���K�[�p�ɕύX�B
			physx::PxShapeFlags shapeFlags =
				physx::PxShapeFlag::eVISUALIZATION |
				physx::PxShapeFlag::eTRIGGER_SHAPE |
				physx::PxShapeFlag::eSCENE_QUERY_SHAPE;

			physx::PxShape* shapes[1];
			m_rigidTrigger->getShapes(shapes, 1);
			shapes[0]->setFlags(shapeFlags);

			if (m_rigidTrigger == nullptr)
			{
				return false;
			}

			InitRigidActor(m_rigidTrigger);


			return true;
		}

		void CPhysicsTriggerObject::VirtualActivate() noexcept
		{
			CPhysicsEngine::GetInstance()->AddPhysicsTriggerObject(this);
			return;
		}

		CPhysicsTriggerObject::~CPhysicsTriggerObject()
		{
			Terminate();
			return;
		}

		void CPhysicsTriggerObject::Terminate()
		{
			Release();
			return;
		}

		void CPhysicsTriggerObject::Release()
		{
			if (m_rigidTrigger == nullptr)
			{
				return;
			}

			ReleaseRigidActor();
			m_rigidTrigger = nullptr;

			return;
		}

		void CPhysicsTriggerObject::UpdateContactEvent()
		{
			if (m_rigidTrigger == nullptr)
			{
				// ���������Ȃ���s���Ȃ��B
				return;
			}

			constexpr int bufferSize = 128;
			physx::PxOverlapHit hitBuffers[bufferSize];
			physx::PxOverlapBuffer hitCallback(hitBuffers, bufferSize);

			bool isHit =
				nsPhysics::CPhysicsEngine::GetInstance()->OverlapMultiple(*this, &hitCallback);

			if (isHit == false)
			{
				return;
			}

			auto* myExtendedData =
				static_cast<CExtendedDataForRigidActor*>(GetActor()->userData);

			const uint32_t numHits = hitCallback.getNbAnyHits();
			for (uint32_t hitIdx = 0; hitIdx < numHits; hitIdx++)
			{
				auto& hitActor = hitCallback.getAnyHit(hitIdx).actor;
				if (hitActor == GetActor())
				{
					// �������g�̓q�b�g���肵�Ȃ��B
					continue;
				}

				if (hitActor->is<physx::PxRigidDynamic>())
				{
					// Dynamic�́AContactReportCallBack�Ńq�b�g���肳��邽�߁A
					// �����ł̓q�b�g���肵�Ȃ��B
					continue;
				}

				// �t�ɁATrigger��Static�́AContactReportCallBack�Ŗ�������邽�߁A
				// �����Ńq�b�g������s���B

				if (hitActor->userData)
				{
					auto* otherExtendedData =
						static_cast<CExtendedDataForRigidActor*>(hitActor->userData);

					if (m_extendedDataMapInContact.count(otherExtendedData) == 0)
					{
						// Map�Ƀf�[�^���Ȃ����߁A�g���K�[�̒��ɓ������u�ԁB
						myExtendedData->GetContactEvent()->m_onTriggerEnterFunc(otherExtendedData);
						// Map�Ƀf�[�^��ǉ����A���݂̃t���[���ł��ڐG���Ă����ԂɃ}�[�N�B
						m_extendedDataMapInContact.emplace(otherExtendedData, true);
					}
					else
					{
						// Map�ɂ��łɃf�[�^�����邽�߁A�g���K�[�̒��ɂ����ԁB
						myExtendedData->GetContactEvent()->m_onTriggerStayFunc(otherExtendedData);
						// ���݂̃t���[���ł��ڐG���Ă����ԂɃ}�[�N�B
						m_extendedDataMapInContact.at(otherExtendedData) = true;
					}

				}
			}

			for (auto it = m_extendedDataMapInContact.begin(); it != m_extendedDataMapInContact.end();)
			{
				if (it->second == false)
				{
					// Map�Ƀf�[�^�͂��邪�A���݂̃t���[���ł͐ڐG���Ă��Ȃ�������ԁB
					// �܂�A�g���K�[����o�čs�����u�ԁB
					myExtendedData->GetContactEvent()->m_onTriggerExitFunc(it->first);
					// �g���K�[����o�čs�����̂ŁA�ڐG���̃f�[�^��ێ����邱�̃}�b�v�����������B
					it = m_extendedDataMapInContact.erase(it);
				}
				else
				{
					// Map�Ƀf�[�^������A���A���݂̃t���[���ł��ڐG���Ă�����ԁB
					// ���݂̃t���[���ł̐ڐG�t���O�����Z�b�g�B
					it->second = false;
					it++;
				}
			}

			return;
		}

		void CPhysicsTriggerObject::SetOnTriggerEnterFunc(
			const SContactEvent::ContactEventFunc& func) noexcept
		{
			auto* extendedData = GetExtendedData();
			if (extendedData == nullptr)
			{
				return;
			}

			extendedData->GetContactEvent()->m_onTriggerEnterFunc = func;

			if (m_rigidTrigger != nullptr && m_rigidTrigger->userData == nullptr)
			{
				m_rigidTrigger->userData = static_cast<void*>(extendedData);
			}

			return;
		}

		void CPhysicsTriggerObject::SetOnTriggerExitFunc(
			const SContactEvent::ContactEventFunc& func) noexcept
		{
			auto* extendedData = GetExtendedData();
			if (extendedData == nullptr)
			{
				return;
			}

			extendedData->GetContactEvent()->m_onTriggerExitFunc = func;

			if (m_rigidTrigger != nullptr && m_rigidTrigger->userData == nullptr)
			{
				m_rigidTrigger->userData = static_cast<void*>(extendedData);
			}

			return;
		}

		void CPhysicsTriggerObject::SetOnTriggerStayFunc(
			const SContactEvent::ContactEventFunc& func) noexcept
		{
			auto* extendedData = GetExtendedData();
			if (extendedData == nullptr)
			{
				return;
			}

			extendedData->GetContactEvent()->m_onTriggerStayFunc = func;

			if (m_rigidTrigger != nullptr && m_rigidTrigger->userData == nullptr)
			{
				m_rigidTrigger->userData = static_cast<void*>(extendedData);
			}

			return;
		}

	}
}