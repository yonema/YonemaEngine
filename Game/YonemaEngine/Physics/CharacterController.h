#pragma once
#include "PhysicsStaticObject.h"
#include <PxQueryFiltering.h>

namespace physx
{
	class PxRigidActor;
}

namespace nsYMEngine
{
	namespace nsPhysics
	{
		class CCharacterController : public nsGameObject::IGameObject
		{
		private:

			class CSweepFilterCallback : public physx::PxQueryFilterCallback
			{
			public:
				physx::PxQueryHitType::Enum preFilter(
					const physx::PxFilterData& filterData,
					const physx::PxShape* shape,
					const physx::PxRigidActor* actor,
					physx::PxHitFlags& queryFlags
				) override final
				{
					if (m_actor == actor)
					{
						return physx::PxQueryHitType::eNONE;
					}

					return physx::PxQueryHitType::eTOUCH;
				};

				physx::PxQueryHitType::Enum postFilter(
					const physx::PxFilterData& filterData,
					const physx::PxQueryHit& hit
				) override final
				{
					return physx::PxQueryHitType::eBLOCK;
				};

			public:
				constexpr CSweepFilterCallback() = default;
				~CSweepFilterCallback() = default;

				constexpr void SetActor(const physx::PxRigidActor* actor)
				{
					m_actor = actor;
				}

			private:
				const physx::PxRigidActor* m_actor = nullptr;
			};


		private:
			static const unsigned int m_kNumCollisionCalculations = 5;
			static const unsigned int m_kHitBufferSize = 32;
			static const float m_kPenetrationDepthThreshold;
			static const float m_kDefaultMaxGravityFactor;


		public:
			bool Start() override final { /* Update‚Í•s—v */ return false; };
			//void Update(float deltaTime) override final;
			void OnDestroy() override final;

		public:
			CCharacterController();
			~CCharacterController() = default;

			void Release();

			bool Init(
				float radius,
				float halfHeight,
				const nsMath::CVector3& position,
				float heightPivot = 0.0f,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enCharacterController
			);

			template <class OwnerClass>
			bool Init(
				OwnerClass* owner,
				float radius,
				float halfHeight,
				const nsMath::CVector3& position,
				float heightPivot = 0.0f,
				EnPhysicsAttributes attribute = EnPhysicsAttributes::enCharacterController
			)
			{
				bool res = 
					m_characterCapsule.InitAsCapsule<OwnerClass>(
						owner,
						radius,
						halfHeight,
						position,
						EnPhysicsAttributes::enCharacterController
						);

				if (res != true)
				{
					return false;
				}

				InitCore(radius, halfHeight, position, heightPivot);

				return true;
			}

			const nsMath::CVector3& Execute(const nsMath::CVector3& moveVec, float deltaTime) noexcept;

			constexpr const nsMath::CVector3& GetPosition() const noexcept
			{
				return m_position;
			}

			constexpr bool IsOnGround() const noexcept
			{
				return m_isOnGround;
			}

			constexpr bool IsOnWall() const noexcept
			{
				return m_isOnWall;
			}

			constexpr bool IsEnableGravity() const noexcept
			{
				return m_enableGravity;
			}

			constexpr void EnableGravity() noexcept
			{
				m_enableGravity = true;
			}

			constexpr void DisableGravity() noexcept
			{
				m_enableGravity = false;
			}

			constexpr float GetGravityScale() const noexcept
			{
				return m_gravityScale;
			}

			constexpr void SetGravityScale(float gravityScale) noexcept
			{
				m_gravityScale = gravityScale;
			}

			constexpr float GetMaxGravityFactor() const noexcept
			{
				return m_maxGravityFactor;
			}

			constexpr void SetMaxGravityFactor(float maxGravityFactor) noexcept
			{
				m_maxGravityFactor = maxGravityFactor;
			}


		private:

			void Terminate();

			void InitCore(
				float radius,
				float halfHeight,
				const nsMath::CVector3& position,
				float heightPivot
			);

			void UpdateGravity();

			void ResetFlag();

			nsMath::CVector3 HorizontalCollisionCalculation(
				const nsMath::CVector3& nextPos, const nsMath::CVector3& originMoveDir) noexcept;

			nsMath::CVector3 VerticalCollisionCalculation(const nsMath::CVector3& nextPos) noexcept;

		private:
			CPhysicsStaticObject m_characterCapsule;
			nsMath::CVector3 m_position = nsMath::CVector3::Zero();
			CSweepFilterCallback m_sweepFilterCallback = {};
			const physx::PxRigidActor* m_myActor = nullptr;
			float m_gravityFactor = 0.0f;
			bool m_enableGravity = true;
			float m_gravityScale = 1.0f;
			float m_maxGravityFactor = m_kDefaultMaxGravityFactor;
			bool m_isOnGround = false;
			bool m_isOnWall = false;
			float m_radius = 0.0f;
			float m_halfHeight = 0.0f;
			float m_walkableFloorRadAngle = nsMath::DegToRad(45.0f);
		};

	}
}