#pragma once
namespace nsYMEngine
{
	namespace nsUtils
	{
		class CRandom : private SNoncopyable
		{
		public:
			CRandom();
			~CRandom() = default;

			void Seed() noexcept;

			int GetRangeInt(int min, int max) noexcept;

			double GetRangeDouble(double min, double max) noexcept;

			float GetRangeFloat(float min, float max) noexcept;

		private:
			std::mt19937 m_randGenerator = {};	// Œˆ’è“I‚È—”(‹^——”)¶¬Ší
		};

	}
}