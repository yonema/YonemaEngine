#include "Random.h"

namespace nsYMEngine
{
	namespace nsUtils
	{
		CRandom::CRandom()
		{
			Seed();

			return;
		}

		void CRandom::Seed() noexcept
		{
			// 非決定的な乱数生成器
			std::random_device rnd;

			// 非決定的な乱数を使って、決定的な乱数のシード値を指定する。
			m_randGenerator.seed(rnd());

			return;
		}

		int CRandom::GetRangeInt(int min, int max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"乱数の範囲の最小値に最大値以上の値が指定されています。");
				return min;
			}
#endif // _DEBUG

			// 指定範囲の一様乱数
			std::uniform_int_distribution<> randRange(min, max);
			return randRange(m_randGenerator);
		}

		double CRandom::GetRangeDouble(double min, double max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"乱数の範囲の最小値に最大値以上の値が指定されています。");
				return min;
			}
#endif // _DEBUG

			// 指定範囲の一様乱数
			std::uniform_real_distribution<> randRange(min, max);
			return randRange(m_randGenerator);
		}

		float CRandom::GetRangeFloat(float min, float max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"乱数の範囲の最小値に最大値以上の値が指定されています。");
				return min;
			}
#endif // _DEBUG

			// 指定範囲の一様乱数
			std::uniform_real_distribution<> randRange(
				static_cast<double>(min), static_cast<double>(max));
			return static_cast<float>(randRange(m_randGenerator));
		}



	}
}