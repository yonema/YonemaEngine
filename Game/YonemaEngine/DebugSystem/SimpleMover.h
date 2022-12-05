#pragma once
#include "../GameObject/GameObject.h"

namespace nsYMEngine
{
	namespace nsDebugSystem
	{
		class CSimpleMover : public nsGameObject::IGameObject
		{
		public:
			bool Start() override final;
			void OnDestroy() override final;
			void Update(float deltaTime) override final;

		public:
			constexpr CSimpleMover() = default;
			~CSimpleMover() = default;

			constexpr const nsMath::CVector3& GetPosition() const noexcept
			{
				return m_position;
			}
			inline void SetPosition(const nsMath::CVector3& position) noexcept
			{
				m_position = position;
			}

			constexpr const nsMath::CQuaternion& GetRotation() const noexcept
			{
				return m_rotation;
			}
			inline void SetRotation(const nsMath::CQuaternion& rotation) noexcept
			{
				m_rotation = rotation;
			}

			constexpr float GetMoveSpeed() const noexcept
			{
				return m_moveSpeed;
			}
			constexpr void SetMoveSpeed(float moveSpeed) noexcept
			{
				m_moveSpeed = moveSpeed;
			}
			constexpr float GetTurnSpeed() const noexcept
			{
				return m_turnSpeed;
			}
			constexpr void SetTurnSpeed(float turnSpeed) noexcept
			{
				m_turnSpeed = turnSpeed;
			}

			constexpr const nsMath::CVector3& GetForward() const noexcept
			{
				return m_forward;
			}

			constexpr const nsMath::CVector3& GetRight() const noexcept
			{
				return m_right;
			}

		private:
			void CalcForward() noexcept;

			void CalcRight() noexcept;

			void Turn(float deltaTime) noexcept;

			void Move(float deltaTime) noexcept;

		private:
			nsMath::CVector3 m_position = nsMath::CVector3::Zero();
			nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
			nsMath::CVector3 m_forward = nsMath::CVector3::Front();
			nsMath::CVector3 m_right = nsMath::CVector3::Right();
			float m_moveSpeed = 50.0f;
			float m_turnSpeed = 100.0f;
		};
	}
}
