#pragma once

namespace nsYMEngine
{
	class CYonemaEngine;
	namespace nsGameWindow
	{
		class CGameWindow;
	}
}

namespace nsYMEngine
{
	class CApplication : private nsUtils::SNoncopyable
	{
	private:
		static const int m_kWindowWidth;
		static const int m_kWindowHeight;

	private:
		CApplication() = default;
		~CApplication();

	public:
		inline static void CreateInstance()
		{
			if (m_instance == nullptr)
			{
				m_instance = new CApplication();
			}
		}
		inline static void DeleteInstance()
		{
			if (m_instance != nullptr)
			{
				delete m_instance;
			}
		}
		inline static CApplication* GetInstance()
		{
			return m_instance;
		}

		inline static int GetWindowWidth()
		{
			return m_kWindowWidth;
		}
		inline static int GetWindowHeight()
		{
			return m_kWindowHeight;
		}


		bool Init();

		void Run(int nCmdShow);

	private:
		void Terminate();


	private:
		static CApplication* m_instance;
		CYonemaEngine* m_ymEngine = nullptr;
		nsGameWindow::CGameWindow* m_gameWindow = nullptr;
	};
}
