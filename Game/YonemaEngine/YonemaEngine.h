#pragma once
#include "Time/GameTime.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine; 
		namespace nsAnimations
		{
			class CUpdateAnimationManager;
		}
	}
	namespace nsInput
	{
		class CInputManager;
	}
	namespace nsDebugSystem
	{
		class CDisplayFPS;
	}
	namespace nsPhysics
	{
		class CPhysicsEngine;
	}
	namespace nsEffect
	{
		class CEffectEngine;
	}
	namespace nsSound
	{
		class CSoundEngine;
	}
	namespace nsMemory
	{
		class CResourceBankTable;
	}
	namespace nsUtils
	{
		class CRandom;
	}
	namespace nsThread
	{
		class CLoadModelThread;
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

		constexpr nsUtils::CRandom* GetRandom() noexcept
		{
			return m_random;
		}

	private:

		void Terminate();

	private:
		static CYonemaEngine* m_instance;
		nsGraphics::CGraphicsEngine* m_graphicsEngine = nullptr;
		nsGameObject::CGameObjectManager* m_gameObjectManager = nullptr;
		nsInput::CInputManager* m_inputManager = nullptr;
		nsPhysics::CPhysicsEngine* m_physicsWorld = nullptr;
		nsEffect::CEffectEngine* m_effectEngine = nullptr;
		nsSound::CSoundEngine* m_soundEngine = nullptr;
		nsMemory::CResourceBankTable* m_resourceBankTable = nullptr;
		nsGraphics::nsAnimations::CUpdateAnimationManager* m_updateAnimationManager = nullptr;
		nsTime::CGameTime m_gameTime;
		nsUtils::CRandom* m_random = nullptr;
		nsThread::CLoadModelThread* m_loadModelThread = nullptr;
		bool m_isExitGame = false;

		nsDebugSystem::CDisplayFPS* m_displayFPS = nullptr;
	};
}

