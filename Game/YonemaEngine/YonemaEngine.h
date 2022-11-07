#pragma once
#include "Time/GameTime.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine; 
	}
	namespace nsInput
	{
		class CInputManager;
	}
}

namespace nsYMEngine
{
	class CYonemaEngine : private nsUtils::SNoncopyable
	{
	private:
		CYonemaEngine() = default;
		~CYonemaEngine();

	public:
		
		inline static CYonemaEngine* CreateInstance()
		{
			if (m_instance == nullptr)
			{
				m_instance = new CYonemaEngine;
			}

			return m_instance;
		}
		inline static void DeleteInstance()
		{
			if (m_instance != nullptr)
			{
				delete m_instance;
				m_instance = nullptr;
			}
		}
		constexpr static CYonemaEngine* GetInstance() noexcept
		{
			return m_instance;
		}


		bool Init();

		void Update();

		constexpr float GetDeltaTime() const noexcept
		{
			return m_gameTime.GetDeltaTime();
		}

		inline void ExitGame()
		{
			if (m_isExitGame != true)
			{
				PostQuitMessage(0);
				m_isExitGame = true;
			}
			return;
		}

		constexpr const nsInput::CInputManager* const GetInput()const noexcept
		{
			return m_inputManager;
		}

	private:

		void Terminate();

	private:
		static CYonemaEngine* m_instance;
		nsGraphics::CGraphicsEngine* m_graphicsEngine = nullptr;
		nsGameObject::CGameObjectManager* m_gameObjectManager = nullptr;
		nsInput::CInputManager* m_inputManager = nullptr;
		nsTime::CGameTime m_gameTime;
		bool m_isExitGame = false;
	};
}

