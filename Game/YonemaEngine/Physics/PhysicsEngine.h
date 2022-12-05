#pragma once
#include <PxPhysicsAPI.h>
#include "ContactReportCallback.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsDebugRenderers
		{
			struct PxRaycastHit;
			template<typename HitType>
			struct PxHitBuffer;
			typedef PxHitBuffer<PxRaycastHit> PxRaycastBuffer;

			class CPhysicsDebugLineRenderer;
		}
	}
	namespace nsPhysics
	{
		class IPhysicsObject;
		class CPhysicsTriggerObject;
	}
}
namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CPhysicsEngine : nsUtils::SNoncopyable
		{
		private:
			static const bool m_kEnableCCD;
			static const float m_kGravityStrength;
			static const float m_kDefaultStaticFriction;
			static const float m_kDefaultDynamicFriction;
			static const float m_kDefaultRestitution;
#ifdef _DEBUG
			static const unsigned int m_kMaxMyDebugLine;
#endif // _DEBUG

			struct SMyDebugLine
			{
				constexpr SMyDebugLine() = default;
				constexpr SMyDebugLine(
					const nsMath::CVector3& pos0,
					const nsMath::CVector3& color0,
					const nsMath::CVector3& pos1,
					const nsMath::CVector3& color1
				) noexcept
					:pos0(pos0), color0(color0), pos1(pos1), color1(color1)
				{};
				~SMyDebugLine() = default;

				nsMath::CVector3 pos0;
				nsMath::CVector3 color0;
				nsMath::CVector3 pos1;
				nsMath::CVector3 color1;
			};

		private:
			CPhysicsEngine();
			~CPhysicsEngine();

		public:
			inline static CPhysicsEngine* CreateInstance()
			{
				return m_instance ? m_instance : m_instance = new CPhysicsEngine();
			}
			inline static void DeleteInstance()
			{
				if (m_instance)
				{
					delete m_instance;
					m_instance = nullptr;
				}
			}
			inline static CPhysicsEngine* GetInstance()
			{
				return m_instance;
			}

			inline static bool UsesCCD() noexcept
			{
				return m_kEnableCCD;
			}


			bool Init();

			void Update(float deltaTime);

			inline void AddActor(physx::PxActor& actor)
			{
				AddActorCore(actor);
			}
			inline void AddActor(physx::PxActor* actor)
			{
				AddActorCore(*actor);
			}
			inline void RemoveActor(physx::PxActor& actor)
			{
				RemoveActorCore(actor);
			}
			inline void RemoveActor(physx::PxActor* actor)
			{
				RemoveActorCore(*actor);
			}

			constexpr auto* GetPhysics() noexcept
			{
				return m_physics;
			}

			constexpr auto* GetDefaultMaterial() noexcept
			{
				return m_defaultMaterial;
			}

			constexpr auto* GetCooking() noexcept
			{
				return m_cooking;
			}

			bool RayCast(
				const nsMath::CVector3& origin,
				const nsMath::CVector3& normalDir,
				float distance,
				physx::PxRaycastBuffer* hitCallback,
				const nsMath::CVector4& color = nsMath::CVector4::Red()
			);

			bool OverlapMultiple(const IPhysicsObject& physicsObject, physx::PxOverlapBuffer* hitCallback);

			void AddPhysicsTriggerObject(CPhysicsTriggerObject* object);

			void RemovePhysicsTriggerObject(CPhysicsTriggerObject* object);


			// DebugSystem

			constexpr void EnableDebugDrawLine() noexcept
			{
				m_enableDebugDrawLine = true;
			}
			constexpr void DisableDebugDrawLine() noexcept
			{
				m_enableDebugDrawLine = false;
			}
			constexpr bool IsEnableDebugDrawLine() noexcept
			{
				return m_enableDebugDrawLine;
			}

			void SetCullingBoxForDebugDrawLine(
				float halfExtent, const nsMath::CVector3& center) noexcept;

			constexpr void EnableAutoFitCullingBoxToMainCamera() noexcept
			{
				m_enableAutoFitCullingBoxToMainCamera = true;
			}
			constexpr void DisableAutoFitCullingBoxToMainCamera() noexcept
			{
				m_enableAutoFitCullingBoxToMainCamera = false;
			}
			constexpr bool IsEnableAutoFitCullingBoxToMainCamera() noexcept
			{
				return m_enableAutoFitCullingBoxToMainCamera;
			}

			// End DebugSystem

		private:
			void Terminate();

			bool InitPhysics();

			bool InitScene();

			inline void AddActorCore(physx::PxActor& actor)
			{
				if (m_scene)
				{
					m_scene->addActor(actor);
				}
			}
			inline void RemoveActorCore(physx::PxActor& actor)
			{
				if (m_scene)
				{
					m_scene->removeActor(actor);
				}
			}


			// DebugSystem

			void InitDebugging();

			void UpdateDebugData();

			void AutoFitCullingBoxToMainCamere();

			// End DebugSystem

		private:
			static CPhysicsEngine* m_instance;

			static physx::PxDefaultAllocator m_defaultAllocatorCallback;
			static physx::PxDefaultErrorCallback m_defaultErrorCallback;
			static CContactReportCallback m_contactReportCallback;

			physx::PxFoundation* m_foundation = nullptr;
			physx::PxPvd* m_pvd = nullptr;
			physx::PxPhysics* m_physics = nullptr;
			physx::PxCooking* m_cooking = nullptr;
			physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
			physx::PxScene* m_scene = nullptr;
			physx::PxMaterial* m_defaultMaterial = nullptr;

			std::list<CPhysicsTriggerObject*> m_physicsTriggerObjectList = {};

#ifdef _DEBUG
			bool m_enableDebugDrawLine = false;
			bool m_enableAutoFitCullingBoxToMainCamera = false;
			float m_cullingBoxHalfExtent = 0.0f;
			nsGraphics::nsDebugRenderers::CPhysicsDebugLineRenderer* m_physicsDebugLineRenderer = nullptr;
			std::vector<SMyDebugLine> m_myDebugLineArray = {};
#endif // _DEBUG
		};
	}
}
