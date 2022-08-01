#include <Windows.h>
#ifdef _DEBUG
#include	<iostream>
#endif


/**
 * @brief コンソール画面にフォーマット付き文字列を表示
 * @param format フォーマット（%dとか%fとかの）付き文字列
 * @param  可変長引数
 * @remarks この関数はデバック用です。デバック時にしか動作しません。
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
	// ウィンドウが破棄されたときに呼ばれる。
	if (msg == WM_DESTROY)
	{
		// OSにアプリの終了を通知。
		PostQuitMessage(0);
		return 0;
	}

	// 既定の処理を行う。
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

	// ウィンドウクラスの作成と登録。
	WNDCLASSEX w = { };
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// コールバック関数の指定。
	w.lpszClassName = L"GameSample";			// アプリケーションクラス名（適当でよい）。
	w.hInstance = GetModuleHandle(nullptr);		// ハンドルの取得。

	// アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）。
	RegisterClassEx(&w);

	// ウィンドウのサイズを決める。
	RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
	// 関数を使ってウィンドウのサイズを補正する。
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成。
	HWND hwnd = CreateWindow(
		w.lpszClassName,		// クラス名指定
		L"GameSample",			// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	// タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			// 表示x座標。OSにおまかせ。
		CW_USEDEFAULT,			// 表示y座標。OSにおまかせ。
		wrc.right - wrc.left,	// ウィンドウ幅
		wrc.bottom - wrc.top,	// ウィンドウ高
		nullptr,				// 親ウィンドウハンドル
		nullptr,				// メニューハンドル
		w.hInstance,			// 呼び出しアプリケーションハンドル
		nullptr,				// 追加パラメータ
		);

	// ウィンドウ表示
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

	// もうクラスは使わないので、登録解除する。
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}