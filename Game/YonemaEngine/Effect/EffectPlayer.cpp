#include "EffectPlayer.h"
#include "EffectEngine.h"

namespace nsYMEngine
{
	namespace nsEffect
	{
		bool CEffectPlayer::Start()
		{
			return true;
		}

		void CEffectPlayer::OnDestroy()
		{
			return;
		}

		void CEffectPlayer::Update(float deltaTime)
		{
			UpdateWorldMatrix();

			return;
		}

		bool CEffectPlayer::Init(const wchar_t* filePath)
		{
			m_effectRef = CEffectEngine::GetInstance()->RegisterEffect(filePath);

			return true;
		}

		void CEffectPlayer::Play()
		{
			Stop();
			m_instanceHandle = CEffectEngine::GetInstance()->Play(m_effectRef, m_position);
		}

		bool CEffectPlayer::IsPlaying() const
		{
			return CEffectEngine::GetInstance()->IsPlaying(m_instanceHandle);
		}


		void CEffectPlayer::Stop()
		{
			CEffectEngine::GetInstance()->Stop(m_instanceHandle);
		}

		void CEffectPlayer::Pause()
		{
			CEffectEngine::GetInstance()->Pause(m_instanceHandle);
		}

		bool CEffectPlayer::IsPaused() const
		{
			return CEffectEngine::GetInstance()->IsPaused(m_instanceHandle);
		}

		void CEffectPlayer::SetSpeed(float speed)
		{
			CEffectEngine::GetInstance()->SetSpeed(m_instanceHandle, speed);
		}

		float CEffectPlayer::GetSpeed() const
		{
			return CEffectEngine::GetInstance()->GetSpeed(m_instanceHandle);
		}

		void CEffectPlayer::UpdateWorldMatrix()
		{
			if (m_isDirty != true)
			{
				return;
			}

			CEffectEngine::GetInstance()->SetPosition(m_instanceHandle, m_position);

			const float halfRadAngle = acosf(m_rotation.w);
			const float radAngle = 2.0f * halfRadAngle;
			nsMath::CVector3 axis(nsMath::CVector3::Up());

			// 0Š„‚è—\–h
			if (fabsf(halfRadAngle) > FLT_EPSILON)
			{
				axis.x = m_rotation.x / sinf(halfRadAngle);
				axis.y = m_rotation.y / sinf(halfRadAngle);
				axis.z = m_rotation.z / sinf(halfRadAngle);
				axis.Normalize();
			}

			axis.x = m_rotation.x / sinf(halfRadAngle);
			axis.y = m_rotation.y / sinf(halfRadAngle);
			axis.z = m_rotation.z / sinf(halfRadAngle);
			axis.Normalize();

			CEffectEngine::GetInstance()->SetRotation(m_instanceHandle, axis, radAngle);
			CEffectEngine::GetInstance()->SetScale(m_instanceHandle, m_scale);

			m_isDirty = false;
		}


	}
}