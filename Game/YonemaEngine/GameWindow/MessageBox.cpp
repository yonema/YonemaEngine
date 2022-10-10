#include "GameWindow.h"

namespace nsYMEngine
{
	namespace nsGameWindow
	{
		/**
		 * @brief 警告のメッセージボックスを表示します
		 * @param[in] text 表示するテキスト
		*/
		void MessageBoxWarning(const wchar_t* text)
		{
			HWND hwnd = CGameWindow::GetInstance()->GetHWND();
			MessageBox(hwnd, text, L"警告", MB_OK | MB_ICONWARNING);
		}

		/**
		 * @brief エラーのメッセージボックスを表示します
		 * @param[in] text 表示するテキスト
		*/
		void MessageBoxError(const wchar_t* text)
		{
			HWND hwnd = CGameWindow::GetInstance()->GetHWND();
			MessageBox(hwnd, text, L"エラー", MB_OK | MB_ICONERROR);
		}

	}
}