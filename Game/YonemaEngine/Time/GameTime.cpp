#include "GameTime.h"

namespace nsYMEngine
{
	namespace nsTime
	{
		const float CGameTime::m_kLatestDeltaTime = 1.0f / 30.0f;
		const int CGameTime::m_kMaxDeltaTimeQueSize = 29;

		void CGameTime::EndTimeMeasurement() noexcept
		{
			m_timer.EndTimer();
			PushDeltaTimeToQue(static_cast<float>(m_timer.GetSeconds()));
			return;
		}

		void CGameTime::CalcDeltaTime() noexcept
		{
			m_timer.EndTimer();
			PushDeltaTimeToQue(static_cast<float>(m_timer.GetSeconds()));
			m_timer.StartTimer();

			return;
		}


		void CGameTime::PushDeltaTimeToQue(float deltaTime) noexcept
		{
			m_deltaTimeQue.emplace_back(deltaTime);

			const auto deltaTimeQueSize = m_deltaTimeQue.size();
			if (deltaTimeQueSize >= m_kMaxDeltaTimeQueSize)
			{
				float totalTime = 0.0f;
				for (auto deltaTime : m_deltaTimeQue)
				{
					totalTime += deltaTime;
				}
				m_deltaTime = std::min(m_kLatestDeltaTime, totalTime / static_cast<float>(deltaTimeQueSize));
				m_deltaTimeQue.pop_front();
			}
			return;
		}


	}
}