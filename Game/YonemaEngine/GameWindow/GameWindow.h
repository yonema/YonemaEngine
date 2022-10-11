#pragma once

namespace nsYMEngine
{
	namespace nsGameWindow
	{
		class CGameWindow : private nsUtils::SNoncopyable
		{
		private:
			CGameWindow(const int kWindowWidth, const int kWindowHeight);
			~CGameWindow();

		public:
			
			inline static CGameWindow* CreateInstance(const int kWindowWidth, const int kWindowHeight)
			{
				if (m_instance == nullptr)
				{
					m_instance = new CGameWindow(kWindowWidth, kWindowHeight);
				}

				return m_instance;
			}
			inline static void DeleteInstance()
			{
				if (m_instance != nullptr)
				{
					delete m_instance;
				}
			}
			inline static CGameWindow* GetInstance()
			{
				return m_instance;
			}

			void DisplayWindow(int nCmdShow);

			/**
			 * @brief ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
			 * @return ゲームループを継続するか？
			 * @retval true ゲームループ続行
			 * @retval false ゲーム終了
			*/
			bool DispatchWindowMessage();

			HWND GetHWND() const 
			{
				return m_hwnd;
			}

		private:
			static CGameWindow* m_instance;
			HWND m_hwnd;
			WNDCLASSEX m_windClassEx;
		};
	}
}