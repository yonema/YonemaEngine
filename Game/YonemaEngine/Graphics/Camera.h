#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CCamera
		{
		public:
			
			enum class EnProjectionType
			{
				enPerspective,
				enOrthographic
			};

		public:
			CCamera() noexcept;
			~CCamera() = default;

			/**
			 * @brief �J�����̍s���x�N�g���Ȃǂ̃p�����[�^���X�V���܂��B
			 * Set�֐��Ńp�����[�^��ύX������A���̊֐���ǂ�ł��������B
			 * �Ă΂Ȃ��ƁA�J�����̃p�����[�^���X�V����܂���B
			*/
			void UpdateCameraParam() noexcept;

			/**
			 * @brief ���[���h���W����X�N���[�����W���v�Z����B\n
			 * �X�N���[�����W�͒��S��{0.0f, 0.0f}�A���オ{��ʂ̕�*-0.5f, ��ʂ̍���*-0.5f}�A
			 * �E����{��ʂ̕�*0.5f, ��ʂ̍���*0.5f}�Ƃ�����W�n�ł��B
			 * @param worldPos ���[���h���W
			 * @return �X�N���[�����W
			*/
			nsMath::CVector2 CalcScreenPositionFromWorldPosition(nsMath::CVector3& worldPos) noexcept;

			inline void SetPosition(const nsMath::CVector3& pos) noexcept
			{
				m_position = pos;
			}
			constexpr const nsMath::CVector3& GetPosition() const noexcept
			{
				return m_position;
			}

			inline void SetTargetPosition(const nsMath::CVector3& targetPos) noexcept
			{
				m_targetPosition = targetPos;
			}
			constexpr const nsMath::CVector3& GetTargetPosition() const noexcept
			{
				return m_targetPosition;
			}

			inline void SetUpDirection(const nsMath::CVector3& upDir) noexcept
			{
				m_upDirection = upDir;
			}
			constexpr const nsMath::CVector3& GetUpDirection() const noexcept
			{
				return m_upDirection;
			}

			constexpr const nsMath::CVector3& GetForeardDirection() const noexcept
			{
				return m_forwardDirection;
			}

			constexpr const nsMath::CVector3& GetRightDirection() const noexcept
			{
				return m_rightDirection;
			}

			constexpr const nsMath::CMatrix& GetViewMatirx() const noexcept
			{
				return m_viewMatrix;
			}
			constexpr const nsMath::CMatrix& GetInvViewMatirx() const noexcept
			{
				return m_invViewMatrix;
			}
			constexpr const nsMath::CMatrix& GetProjectionMatirx() const noexcept
			{
				return m_projectionMatrix;
			}
			constexpr const nsMath::CMatrix& GetViewProjectionMatirx() const noexcept
			{
				return m_viewProjectionMatrix;
			}
			constexpr const nsMath::CMatrix& GetRotationMatirx() const noexcept
			{
				return m_rotationMatrix;
			}

			constexpr void SetProjectionType(EnProjectionType projType) noexcept
			{
				m_projectionType = projType;
			}
			constexpr EnProjectionType GetProjectionType() const noexcept
			{
				return m_projectionType;
			}
			constexpr void SetAspectRatio(float aspect) noexcept
			{
				m_isDirtyProjectionMatrix = true;
				m_aspectRatio = aspect;
			}
			constexpr float GetAspectRatio() const noexcept
			{
				return m_aspectRatio;
			}
			constexpr void SetFieldOfViewAngleY(float fovAngleY) noexcept
			{
				m_isDirtyProjectionMatrix = true;
				m_fieldOfViewAngleY = fovAngleY;
			}
			constexpr float GetFieldOfViewAngleY() const noexcept
			{
				return m_fieldOfViewAngleY;
			}

			constexpr void SetNearClip(float nearClip) noexcept
			{
				if (nearClip >= m_farClip)
				{
					return;
				}
				m_isDirtyProjectionMatrix = true;
				m_nearClip = nearClip;
			}
			constexpr float GetNearClip() const noexcept
			{
				return m_nearClip;
			}

			constexpr void SetFarClip(float farClip) noexcept
			{
				if (farClip <= m_nearClip)
				{
					return;
				}
				m_isDirtyProjectionMatrix = true;
				m_farClip = farClip;
			}
			constexpr float GetFarClip() const noexcept
			{
				return m_farClip;
			}

		private:

			void UpdateProjectionMatrix() noexcept;

		private:
			nsMath::CVector3 m_position = nsMath::CVector3::Back();
			nsMath::CVector3 m_targetPosition = nsMath::CVector3::Front();
			nsMath::CVector3 m_upDirection = nsMath::CVector3::Up();
			nsMath::CVector3 m_forwardDirection = nsMath::CVector3::Front();
			nsMath::CVector3 m_rightDirection = nsMath::CVector3::Right();

			nsMath::CMatrix m_viewMatrix = nsMath::CMatrix::Identity();
			nsMath::CMatrix m_invViewMatrix = nsMath::CMatrix::Identity();
			nsMath::CMatrix m_projectionMatrix = nsMath::CMatrix::Identity();
			nsMath::CMatrix m_viewProjectionMatrix = nsMath::CMatrix::Identity();
			nsMath::CMatrix m_rotationMatrix = nsMath::CMatrix::Identity();

			EnProjectionType m_projectionType = EnProjectionType::enPerspective;
			float m_aspectRatio = 0.0f;
			float m_fieldOfViewAngleY = nsMath::DegToRad(60.0f);
			float m_nearClip = 1.0f;
			float m_farClip = 100.0f;

			bool m_isDirtyProjectionMatrix = true;


		};

	}
}