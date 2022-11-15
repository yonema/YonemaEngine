#include "GameWindow.h"

namespace nsYMEngine
{
	namespace nsGameWindow
	{

		CGameWindow* CGameWindow::m_instance = nullptr;

		/**
		 * @brief ウィンドウプロシージャ。アプリケーションに送られてきたメッセージを処理する。
		 * @param hwnd ウィンドウハンドル
		 * @param msg メッセージコード
		 * @param wparam w-パラメータ
		 * @param lparam l-パラメータ
		 * @return
		*/
		LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			// 送られてきたメッセージで処理を分岐させる。
			switch (msg)
			{
			case WM_DESTROY:	// ウィンドウが破棄されたときに呼ばれる。
				// OSにアプリの終了を通知。
				PostQuitMessage(0);
				break;

			default:
				// 既定の処理を行う。
				return DefWindowProc(hwnd, msg, wparam, lparam);
			}

			return 0;

		}



		CGameWindow::CGameWindow(const int kWindowWidth, const int kWindowHeight)
		{
			// 〇ウィンドウクラスの作成と登録。
			// ウィンドウクラスのパラメータを設定。
			m_windClassEx =
			{
				sizeof(WNDCLASSEX),			// 構造体のサイズ
				CS_CLASSDC,					// ウィンドウのスタイル。
				(WNDPROC)WindowProcedure,	// ウィンドウプロシージャ
				0,
				0,
				GetModuleHandle(nullptr),	// ハンドルの取得
				nullptr,					// アイコンのハンドル
				nullptr,					// マウスカーソルのハンドル
				nullptr,					// ウィンドウの背景色
				nullptr,					// メニュー名
				L"GameSample",				// ウィンドウクラス名（適当でよい）
				nullptr
			};

			// ウィンドウクラスの登録。
			RegisterClassEx(&m_windClassEx);

			// ウィンドウのサイズを決める。
			RECT wrc = { 0, 0, kWindowWidth, kWindowHeight };
			// 関数を使ってウィンドウのサイズを補正する。
			AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

			// 画面のサイズ
			const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
			const int screenHeight = GetSystemMetrics(SM_CYSCREEN);

			// ウィンドウを画面の中央に配置する。
			const int windowPosX = (screenWidth - (wrc.right - wrc.left)) / 2;
			const int windowPosY = (screenHeight - (wrc.bottom - wrc.top)) / 2;

			// ウィンドウオブジェクトの生成。
			m_hwnd = CreateWindow(
				m_windClassEx.lpszClassName,		// 使用するウィンドウクラス名の指定
				L"GameSample",			// タイトルバーの文字。クラス名と違っても大丈夫。
				WS_OVERLAPPEDWINDOW,	// ウィンドウのスタイル。タイトルバーと境界線があるウィンドウ。
				//CW_USEDEFAULT,			// 表示x座標。OSにおまかせ。
				windowPosX,				// 表示x座標。画面中央に配置。
				windowPosY,				// 表示y座標。画面中央に配置。
				wrc.right - wrc.left,	// ウィンドウの幅
				wrc.bottom - wrc.top,	// ウィンドウの高さ
				nullptr,				// 親ウィンドウハンドル
				nullptr,				// メニューハンドル
				m_windClassEx.hInstance,			// 呼び出しアプリケーションハンドル
				nullptr					// 追加パラメータ
			);
			return;
		}


		CGameWindow::~CGameWindow()
		{

			// もうクラスは使わないので、登録解除する。
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
				//ウィンドウからのメッセージを受け取る。
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else 
				{
					//ウィンドウメッセージが空になった。
					break;
				}
			}
			return msg.message != WM_QUIT;
		}


	}
}