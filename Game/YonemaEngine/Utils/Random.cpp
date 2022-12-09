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
			// �񌈒�I�ȗ���������
			std::random_device rnd;

			// �񌈒�I�ȗ������g���āA����I�ȗ����̃V�[�h�l���w�肷��B
			m_randGenerator.seed(rnd());

			return;
		}

		int CRandom::GetRangeInt(int min, int max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"�����͈̔͂̍ŏ��l�ɍő�l�ȏ�̒l���w�肳��Ă��܂��B");
				return min;
			}
#endif // _DEBUG

			// �w��͈͂̈�l����
			std::uniform_int_distribution<> randRange(min, max);
			return randRange(m_randGenerator);
		}

		double CRandom::GetRangeDouble(double min, double max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"�����͈̔͂̍ŏ��l�ɍő�l�ȏ�̒l���w�肳��Ă��܂��B");
				return min;
			}
#endif // _DEBUG

			// �w��͈͂̈�l����
			std::uniform_real_distribution<> randRange(min, max);
			return randRange(m_randGenerator);
		}

		float CRandom::GetRangeFloat(float min, float max) noexcept
		{
#ifdef _DEBUG
			if (min >= max)
			{
				nsGameWindow::MessageBoxWarning(L"�����͈̔͂̍ŏ��l�ɍő�l�ȏ�̒l���w�肳��Ă��܂��B");
				return min;
			}
#endif // _DEBUG

			// �w��͈͂̈�l����
			std::uniform_real_distribution<> randRange(
				static_cast<double>(min), static_cast<double>(max));
			return static_cast<float>(randRange(m_randGenerator));
		}



	}
}