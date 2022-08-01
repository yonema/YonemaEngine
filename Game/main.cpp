#include <Windows.h>
#ifdef _DEBUG
#include	<iostream>
#endif


/**
 * @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
 * @param format �t�H�[�}�b�g�i%d�Ƃ�%f�Ƃ��́j�t��������
 * @param  �ϒ�����
 * @remarks ���̊֐��̓f�o�b�N�p�ł��B�f�o�b�N���ɂ������삵�܂���B
*/
void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif

	return;
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ�Ƃ��ɌĂ΂��B
	if (msg == WM_DESTROY)
	{
		// OS�ɃA�v���̏I����ʒm�B
		PostQuitMessage(0);
		return 0;
	}

	// ����̏������s���B
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	//DebugOutputFormatString("Show window test.");
	//char a = getchar();


	constexpr int kWindowWidth = 1280;
	constexpr int kWindowHeight = 720;

	// �E�B���h�E�N���X�̍쐬�Ɠo�^�B
	WNDCLASSEX w = { };
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// �R�[���o�b�N�֐��̎w��B
	w.lpszClassName = L"GameSample";			// �A�v���P�[�V�����N���X���i�K���ł悢�j�B
	w.hInstance = GetModuleHandle(nullptr);		// �n���h���̎擾�B

	// �A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j�B
	RegisterClassEx(&w);

	// �E�B���h�E�̃T�C�Y�����߂�B
	RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����B
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐����B
	HWND hwnd = CreateWindow(
		w.lpszClassName,		// �N���X���w��
		L"GameSample",			// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			// �\��x���W�BOS�ɂ��܂����B
		CW_USEDEFAULT,			// �\��y���W�BOS�ɂ��܂����B
		wrc.right - wrc.left,	// �E�B���h�E��
		wrc.bottom - wrc.top,	// �E�B���h�E��
		nullptr,				// �e�E�B���h�E�n���h��
		nullptr,				// ���j���[�n���h��
		w.hInstance,			// �Ăяo���A�v���P�[�V�����n���h��
		nullptr,				// �ǉ��p�����[�^
		);

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	// �����N���X�͎g��Ȃ��̂ŁA�o�^��������B
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}