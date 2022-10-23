#pragma once
#include <chrono>

namespace nsYMEngine
{
	namespace nsTime
	{
		class CTimer : private nsUtils::SNoncopyable
		{
		public:
			constexpr CTimer() = default;
			~CTimer() = default;

		public:

			inline void StartTimer() noexcept;

			inline void EndTimer() noexcept;

			constexpr auto GetNanoSeconds() const noexcept
			{
				return m_nanoSeconds;
			}
			constexpr auto GetMicroSeconds() const noexcept
			{
				return m_microSeconds;
			}
			constexpr auto GetMilliSeconds() const noexcept
			{
				return m_milliSeconds;
			}
			constexpr auto GetSeconds() const noexcept
			{
				return m_seconds;
			}

		private:
			std::chrono::system_clock::time_point m_startTimePoint;
			std::chrono::system_clock::time_point m_endTimePoint;
			unsigned long long int m_nanoSeconds = 0;
			double m_microSeconds = 0.0f;
			double m_milliSeconds = 0.0f;
			double m_seconds = 0.0f;
		};

		inline void CTimer::StartTimer() noexcept
		{
			m_startTimePoint = std::chrono::system_clock::now();
			return;
		}

		inline void CTimer::EndTimer() noexcept
		{
			m_endTimePoint = std::chrono::system_clock::now();

			auto duration = m_endTimePoint - m_startTimePoint;
			m_nanoSeconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
			m_microSeconds = static_cast<double>(m_nanoSeconds) / 1000.0;
			m_milliSeconds = m_microSeconds / 1000.0;
			m_seconds = m_milliSeconds / 1000.0;

			return;
		}

	}
}