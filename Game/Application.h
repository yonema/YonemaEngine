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
		constexpr static CApplication* GetInstance() noexcept
		{
			return m_instance;
		}

		constexpr static int GetWindowWidth() noexcept
		{
			return m_kWindowWidth;
		}
		constexpr static int GetWindowHeight() noexcept
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
