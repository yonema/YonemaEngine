#include "GameWindow.h"

namespace nsYMEngine
{
	namespace nsGameWindow
	{

		CGameWindow* CGameWindow::m_instance = nullptr;

		/**
		 * @brief �E�B���h�E�v���V�[�W���B�A�v���P�[�V�����ɑ����Ă������b�Z�[�W����������B
		 * @param hwnd �E�B���h�E�n���h��
		 * @param msg ���b�Z�[�W�R�[�h
		 * @param wparam w-�p�����[�^
		 * @param lparam l-�p�����[�^
		 * @return
		*/
		LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			// �����Ă������b�Z�[�W�ŏ����𕪊򂳂���B
			switch (msg)
			{
			case WM_DESTROY:	// �E�B���h�E���j�����ꂽ�Ƃ��ɌĂ΂��B
				// OS�ɃA�v���̏I����ʒm�B
				PostQuitMessage(0);
				break;

			default:
				// ����̏������s���B
				return DefWindowProc(hwnd, msg, wparam, lparam);
			}

			return 0;

		}



		CGameWindow::CGameWindow(const int kWindowWidth, const int kWindowHeight)
		{
			// �Z�E�B���h�E�N���X�̍쐬�Ɠo�^�B
			// �E�B���h�E�N���X�̃p�����[�^��ݒ�B
			m_windClassEx =
			{
				sizeof(WNDCLASSEX),			// �\���̂̃T�C�Y
				CS_CLASSDC,					// �E�B���h�E�̃X�^�C���B
				(WNDPROC)WindowProcedure,	// �E�B���h�E�v���V�[�W��
				0,
				0,
				GetModuleHandle(nullptr),	// �n���h���̎擾
				nullptr,					// �A�C�R���̃n���h��
				nullptr,					// �}�E�X�J�[�\���̃n���h��
				nullptr,					// �E�B���h�E�̔w�i�F
				nullptr,					// ���j���[��
				L"GameSample",				// �E�B���h�E�N���X���i�K���ł悢�j
				nullptr
			};

			// �E�B���h�E�N���X�̓o�^�B
			RegisterClassEx(&m_windClassEx);

			// �E�B���h�E�̃T�C�Y�����߂�B
			RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
			// �֐����g���ăE�B���h�E�̃T�C�Y��␳����B
			AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

			// ��ʂ̃T�C�Y
			const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			// �E�B���h�E����ʂ̒����ɔz�u����B
			const int windowPosX = (screenWidth - (wrc.right - wrc.left)) / 2;
			const int windowPosY = (screenHeight - (wrc.bottom - wrc.top)) / 2;

			// �E�B���h�E�I�u�W�F�N�g�̐����B
			m_hwnd = CreateWindow(
				m_windClassEx.lpszClassName,		// �g�p����E�B���h�E�N���X���̎w��
				L"GameSample",			// �^�C�g���o�[�̕����B�N���X���ƈ���Ă����v�B
				WS_OVERLAPPEDWINDOW,	// �E�B���h�E�̃X�^�C���B�^�C�g���o�[�Ƌ��E��������E�B���h�E�B
				//CW_USEDEFAULT,			// �\��x���W�BOS�ɂ��܂����B
				windowPosX,				// �\��x���W�B��ʒ����ɔz�u�B
				windowPosY,				// �\��y���W�B��ʒ����ɔz�u�B
				wrc.right - wrc.left,	// �E�B���h�E�̕�
				wrc.bottom - wrc.top,	// �E�B���h�E�̍���
				nullptr,				// �e�E�B���h�E�n���h��
				nullptr,				// ���j���[�n���h��
				m_windClassEx.hInstance,			// �Ăяo���A�v���P�[�V�����n���h��
				nullptr					// �ǉ��p�����[�^
			);
			return;
		}


		CGameWindow::~CGameWindow()
		{

			// �����N���X�͎g��Ȃ��̂ŁA�o�^��������B
			UnregisterClass(m_windClassEx.lpszClassName, m_windClassEx.hInstance);

			return;
		}

		void CGameWindow::DisplayWindow(int nCmdShow)
		{
			ShowWindow(m_hwnd, nCmdShow);

			return;
		}

		bool CGameWindow::DispatchWindowMessage()
		{
			MSG msg = { 0 };
			while (WM_QUIT != msg.message)
			{
				//�E�B���h�E����̃��b�Z�[�W���󂯎��B
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else 
				{
					//�E�B���h�E���b�Z�[�W����ɂȂ����B
					break;
				}
			}
			return msg.message != WM_QUIT;
		}


	}
}