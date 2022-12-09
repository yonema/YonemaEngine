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

			// シェイプのフラグをトリガー用に変更。
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
				// 未初期化なら実行しない。
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
					// 自分自身はヒット判定しない。
					continue;
				}

				if (hitActor->is<physx::PxRigidDynamic>())
				{
					// Dynamicは、ContactReportCallBackでヒット判定されるため、
					// ここではヒット判定しない。
					continue;
				}

				// 逆に、TriggerとStaticは、ContactReportCallBackで無視されるため、
				// ここでヒット判定を行う。

				if (hitActor->userData)
				{
					auto* otherExtendedData =
						static_cast<CExtendedDataForRigidActor*>(hitActor->userData);

					if (m_extendedDataMapInContact.count(otherExtendedData) == 0)
					{
						// Mapにデータがないため、トリガーの中に入った瞬間。
						myExtendedData->GetContactEvent()->m_onTriggerEnterFunc(otherExtendedData);
						// Mapにデータを追加し、現在のフレームでも接触している状態にマーク。
						m_extendedDataMapInContact.emplace(otherExtendedData, true);
					}
					else
					{
						// Mapにすでにデータがあるため、トリガーの中にいる状態。
						myExtendedData->GetContactEvent()->m_onTriggerStayFunc(otherExtendedData);
						// 現在のフレームでも接触している状態にマーク。
						m_extendedDataMapInContact.at(otherExtendedData) = true;
					}

				}
			}

			for (auto it = m_extendedDataMapInContact.begin(); it != m_extendedDataMapInContact.end();)
			{
				if (it->second == false)
				{
					// Mapにデータはあるが、現在のフレームでは接触していなかった状態。
					// つまり、トリガーから出て行った瞬間。
					myExtendedData->GetContactEvent()->m_onTriggerExitFunc(it->first);
					// トリガーから出て行ったので、接触中のデータを保持するこのマップから消去する。
					it = m_extendedDataMapInContact.erase(it);
				}
				else
				{
					// Mapにデータがあり、かつ、現在のフレームでも接触していた状態。
					// 現在のフレームでの接触フラグをリセット。
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