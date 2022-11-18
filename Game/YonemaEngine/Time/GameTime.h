#pragma once
#include "Timer.h"

namespace nsYMEngine
{
	namespace nsTime
	{
		class CGameTime : private nsUtils::SNoncopyable
		{
		private:
			static const float m_kLatestDeltaTime;
			static const int m_kMaxDeltaTimeQueSize;

		public:
			constexpr CGameTime() = default;
			~CGameTime() = default;

		public:

			constexpr float GetDeltaTime() const noexcept
			{
				return m_deltaTime;
			}

			inline void StartTimeMeasurement() noexcept;

			void EndTimeMeasurement() noexcept;

		private:

			void PushDeltaTimeToQue(float deltaTime) noexcept;


		private:
			CTimer m_timer;
			std::list<float> m_deltaTimeQue = {};
			float m_deltaTime = 0.0f;
		};


		inline void CGameTime::StartTimeMeasurement() noexcept
		{
			m_timer.StartTimer();
			return;
		}


	}
}

