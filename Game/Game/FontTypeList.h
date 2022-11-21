#pragma once
/**
* @file FontTypeList.h
* @brief �t�H���g�̎�ނ��w�肷��t�@�C���B
* @details EnFontType�Ńt�H���g�̎�ނ𑝂₵�܂��B
* g_kSpriteFontFilePaths�őΉ�����t�H���g�̃t�@�C���p�X���w�肵�܂��B
*/

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			/**
			 * @brief �t�H���g�̎��
			*/
			enum class EnFontType
			{
				enSimple,
				enDegitalTextbook,




				enNum	// enNum�͏����Ă͂����܂���B
			};

			/**
			 * @brief EnFontType�ɑΉ�����t�H���g�̃t�@�C���p�X
			*/
			static const wchar_t* g_kSpriteFontFilePaths[static_cast<unsigned int>(EnFontType::enNum)] =
			{
				L"Assets/Fonts/simple.spritefont",
				L"Assets/Fonts/degitalTextbook.spritefont"
			};

		}
	}
}