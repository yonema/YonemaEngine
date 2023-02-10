#include "Camera.h"
#include "../../Application.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		CCamera::CCamera() noexcept
		{
			const auto kWindowWidth = CApplication::GetInstance()->GetWindowWidth();
			const auto kWindowHeight = CApplication::GetInstance()->GetWindowHeight();
			m_aspectRatio = static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight);
			m_orthographicProjectionSize = 
			{ static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight) };
			return;
		}

		void CCamera::UpdateCameraParam() noexcept
		{
			m_viewMatrix.MakeViewMatrix(m_position, m_targetPosition, m_upDirection);

			UpdateProjectionMatrix();

			m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
			m_invViewMatrix.Inverse(m_viewMatrix);
			m_forwardDirection = {
				m_invViewMatrix.m_fMat[2][0],
				m_invViewMatrix.m_fMat[2][1],
				m_invViewMatrix.m_fMat[2][2]
			};
			m_rightDirection = { 
				m_invViewMatrix.m_fMat[0][0],
				m_invViewMatrix.m_fMat[0][1],
				m_invViewMatrix.m_fMat[0][2]
			};
			m_rotationMatrix = m_invViewMatrix;
			m_rotationMatrix.m_fMat[3][0] = 0.0f;
			m_rotationMatrix.m_fMat[3][1] = 0.0f;
			m_rotationMatrix.m_fMat[3][2] = 0.0f;
			m_rotationMatrix.m_fMat[3][3] = 1.0f;

			return;
		}



		void CCamera::UpdateProjectionMatrix() noexcept
		{
			if (m_isDirtyProjectionMatrix != true)
			{
				return;
			}

			if (m_projectionType == EnProjectionType::enPerspective)
			{
				m_projectionMatrix.MakeProjectionMatrix(
					m_fieldOfViewAngleY,
					m_aspectRatio,
					m_nearClip,
					m_farClip
				);
			}
			else
			{
				m_projectionMatrix.MakeOrthoProjectionMatrix(
					m_orthographicProjectionSize.x,
					m_orthographicProjectionSize.y,
					m_nearClip,
					m_farClip
				);
			}

			m_isDirtyProjectionMatrix = false;

			return;
		}

		nsMath::CVector2 CCamera::CalcScreenPositionFromWorldPosition(const nsMath::CVector3& worldPos) noexcept
		{
			float half_w = 
				static_cast<float>(CApplication::GetInstance()->GetWindowWidth()) * 0.5f;
			float half_h = 
				static_cast<float>(CApplication::GetInstance()->GetWindowHeight()) * 0.5f;

			nsMath::CVector4 screenPosMatrix(worldPos.x, worldPos.y, worldPos.z, 1.0f);
			m_viewProjectionMatrix.Apply(screenPosMatrix);

			nsMath::CVector2 screenPos;
			screenPos.x = (screenPosMatrix.x / screenPosMatrix.w) * half_w;
			screenPos.y = (screenPosMatrix.y / screenPosMatrix.w) * -half_h;

			return screenPos;
		}

	}
}