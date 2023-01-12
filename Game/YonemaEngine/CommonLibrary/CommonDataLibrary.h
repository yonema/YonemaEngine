#pragma once

namespace nsYMEngine
{
	namespace nsCommonDataLibrary
	{
		// �Q�[���I�u�W�F�N�g�̗D��x�̍ō��l
		static constexpr unsigned int g_kMaxGameObjectPriority = 99;
		// �Q�[���I�u�W�F�N�g�̗D��x�̒��Ԓn
		static constexpr unsigned int g_kMidGameObjectPriority = g_kMaxGameObjectPriority / 2;
		// �Q�[���I�u�W�F�N�g�̗D��x�̍Œ�l
		static constexpr unsigned int g_kMinGameObjectPriority = 0;
	}

	/**
	 * @brief �Q�[���I�u�W�F�N�g�̗D��x�B
	 * �D��x�ɂ���čX�V���Ԃ��ω����܂��B
	 * �D��x�̒l���Ⴂ�Q�[���I�u�W�F�N�g�قǐ�ɁA
	 * �D��x�̒l�������Q�[���I�u�W�F�N�g�قǌ�ɁA
	 * �X�V�������s���܂��B
	*/
	enum class EnGOPriority
	{
		// �D��x�̍Œ�l�B
		// ������Ⴂ�l�̗D��x�͍���Ă͂����܂���B
		enMin = nsCommonDataLibrary::g_kMinGameObjectPriority,




		// �D��x�̒��Ԓl
		enMid = nsCommonDataLibrary::g_kMidGameObjectPriority,





		// �D��x�̍ő�l
		// �����荂���l�̗D��x������Ă͂����܂���B
		enMax = nsCommonDataLibrary::g_kMaxGameObjectPriority
	};

	/**
	 * @brief �X�v���C�g��t�H���g�Ŏg�p����A���J�[
	*/
	enum class EnAnchors
	{
		enTopLeft,			// ����
		enTopCenter,		// ������
		enTopRight,			// �E��
		enMiddleLeft,		// ������
		enMiddleCenter,		// ����
		enMiddleRight,		// �E����
		enBottonLeft,		// ����
		enBottonCenter,		// ������
		enBottonRight		// �����E
	};


	enum class EnLoadingState
	{
		enBeforeLoading,
		enNowLoading,
		enAfterLoading
	};



}