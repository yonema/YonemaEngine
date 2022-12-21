#include "EffectSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		const wchar_t* const CEffectSample::
			m_kEffectFilePathArray[static_cast<int>(EnEffectType::enNum)] =
		{
			L"Assets/Effects/Samples/Laser01.efkefc",
			L"Assets/Effects/Samples/square_g.efkefc",
			L"Assets/Effects/Samples/distortion.efkefc",
			L"Assets/Effects/Samples/TriggerLaser.efkefc",
			L"Assets/Effects/Samples/CullingSprite.efkefc",
		};

		bool CEffectSample::Start()
		{
			for (int i = 0; i < static_cast<int>(EnEffectType::enNum); i++)
			{
				m_effectArray[i] = NewGO<CEffectPlayer>();
				m_effectArray[i]->Init(m_kEffectFilePathArray[i]);
			}

			return true;
		}

		void CEffectSample::OnDestroy()
		{
			for (int i = 0; i < static_cast<int>(EnEffectType::enNum); i++)
			{
				DeleteGO(m_effectArray[i]);
			}
			return;
		}

		void CEffectSample::Update(float deltaTime)
		{
			// タイマー更新
			constexpr float kHalfMaxTime = 10.0f;
			m_timer += deltaTime;
			if (m_timer >= kHalfMaxTime * 2.0f)
			{
				m_timer = 0.0f;
			}

			// ワールド行列更新

			constexpr float kStartX = -20.0f;
			constexpr float kEndX = 20.0f;
			constexpr float kStartScale = 0.5f;
			constexpr float kEndScale = 2.0f;

			float rate = m_timer / kHalfMaxTime;
			if (rate > 1.0f)
			{
				rate = 2.0f - rate;
			}
			const float currentX = nsMath::Lerpf(rate, kStartX, kEndX);
			const float currentScale = nsMath::Lerpf(rate, kStartScale, kEndScale);
			const float currentRot = nsMath::Lerpf(rate, 0.0f, 360.0f);
			nsMath::CQuaternion rot;
			rot.SetRotationYDeg(currentRot);

			for (int i = 0; i < static_cast<int>(EnEffectType::enNum); i++)
			{
				m_effectArray[i]->SetPosition({ currentX, 20.0f, 0.0f });
				m_effectArray[i]->SetRotation(rot);
				m_effectArray[i]->SetScale(currentScale);
			}




			// エフェクト切り替え
			if (Keyboard()->IsTrigger(EnKeyButton::enC))
			{
				m_effectArray[m_currentIdx]->Stop();
				m_currentIdx++;
				if (m_currentIdx >= static_cast<int>(EnEffectType::enNum))
				{
					m_currentIdx = 0;
				}
			}


			// 再生スピード変更
			if (Keyboard()->IsTrigger(EnKeyButton::enF))
			{
				m_effectArray[m_currentIdx]->SetSpeed(
					m_effectArray[m_currentIdx]->GetSpeed() + 0.1f);
			}
			if (Keyboard()->IsTrigger(EnKeyButton::enS))
			{
				m_effectArray[m_currentIdx]->SetSpeed(
					m_effectArray[m_currentIdx]->GetSpeed() - 0.1f);
			}


			// 一時停止
			if (Keyboard()->IsTrigger(EnKeyButton::enP))
			{
				if (m_effectArray[m_currentIdx]->IsPaused() != true)
				{
					m_effectArray[m_currentIdx]->Pause();
				}
				else
				{
					m_effectArray[m_currentIdx]->Play();
				}
			}


			// エフェクト自動再生
			if (Keyboard()->IsTrigger(EnKeyButton::enA))
			{
				m_isAutoPlay = !m_isAutoPlay;
			}

			if (m_isAutoPlay)
			{
				if (m_effectArray[m_currentIdx]->IsPlaying() != true)
				{
					m_effectArray[m_currentIdx]->Play();
				}
			}


			// エフェクト再生
			if (Keyboard()->IsTrigger(EnKeyButton::enSpace))
			{
				m_effectArray[m_currentIdx]->Play();
			}



			return;
		}
	}
}