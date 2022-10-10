#include "GameWindow.h"

namespace nsYMEngine
{
	namespace nsGameWindow
	{
		/**
		 * @brief �x���̃��b�Z�[�W�{�b�N�X��\�����܂�
		 * @param[in] text �\������e�L�X�g
		*/
		void MessageBoxWarning(const wchar_t* text)
		{
			HWND hwnd = CGameWindow::GetInstance()->GetHWND();
			MessageBox(hwnd, text, L"�x��", MB_OK | MB_ICONWARNING);
		}

		/**
		 * @brief �G���[�̃��b�Z�[�W�{�b�N�X��\�����܂�
		 * @param[in] text �\������e�L�X�g
		*/
		void MessageBoxError(const wchar_t* text)
		{
			HWND hwnd = CGameWindow::GetInstance()->GetHWND();
			MessageBox(hwnd, text, L"�G���[", MB_OK | MB_ICONERROR);
		}

	}
}