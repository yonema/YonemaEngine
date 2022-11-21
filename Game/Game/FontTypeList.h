#pragma once
/**
* @file FontTypeList.h
* @brief フォントの種類を指定するファイル。
* @details EnFontTypeでフォントの種類を増やします。
* g_kSpriteFontFilePathsで対応するフォントのファイルパスを指定します。
*/

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFonts
		{
			/**
			 * @brief フォントの種類
			*/
			enum class EnFontType
			{
				enSimple,
				enDegitalTextbook,




				enNum	// enNumは消してはいけません。
			};

			/**
			 * @brief EnFontTypeに対応するフォントのファイルパス
			*/
			static const wchar_t* g_kSpriteFontFilePaths[static_cast<unsigned int>(EnFontType::enNum)] =
			{
				L"Assets/Fonts/simple.spritefont",
				L"Assets/Fonts/degitalTextbook.spritefont"
			};

		}
	}
}