#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{

			/**
			 * @brief �����_���[�̗D��x�B
			 * �D��x�ɂ���čX�V���Ԃ��ω����܂��B
			 * �D��x�̒l���Ⴂ�Q�[���I�u�W�F�N�g�قǐ�ɁA
			 * �D��x�̒l�������Q�[���I�u�W�F�N�g�قǌ�ɁA
			 * �X�V�������s���܂��B
			*/
			enum class EnRendererPriority
			{
				// �D��x�̍Œ�l�B
				// ������Ⴂ�l�̗D��x�͍���Ă͂����܂���B
				enMin,



				// �D��x�̒��Ԓl
				enMid,



				// �D��x�̍ő�l
				// �����荂���l�̗D��x������Ă͂����܂���B
				enMax,
				enNum
			};


			static constexpr unsigned int g_kNumRendererPriority = 
				static_cast<unsigned int>(EnRendererPriority::enNum);

		}
	}
}