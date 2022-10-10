#pragma once

namespace nsYMEngine
{
	namespace nsGameWindow
	{
		/**
		 * @brief 警告のメッセージボックスを表示します
		 * @param[in] text 表示するテキスト
		*/
		void MessageBoxWarning(const wchar_t* text);

		/**
		 * @brief エラーのメッセージボックスを表示します
		 * @param[in] text 表示するテキスト
		*/
		void MessageBoxError(const wchar_t* text);
	}
}
