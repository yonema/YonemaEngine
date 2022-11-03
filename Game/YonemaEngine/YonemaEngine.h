#pragma once
#include "Time/GameTime.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine; 
		namespace ns2D
		{
			class CSprite;
		}
		namespace nsPMDModels
		{
			class CPMDRenderer;
		}
		namespace nsFBXModels
		{
			class CFBXRenderer;
		}
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

	private:

		void Terminate();

	private:
		static CYonemaEngine* m_instance;
		nsGraphics::CGraphicsEngine* m_graphicsEngine = nullptr;
		nsGameObject::CGameObjectManager* m_gameObjectManager = nullptr;
		nsTime::CGameTime m_gameTime;
		bool m_isExitGame = false;
		nsGraphics::nsPMDModels::CPMDRenderer* m_mikuPmdR = nullptr;
		nsGraphics::nsPMDModels::CPMDRenderer* m_mikuMetalPmdR = nullptr;
		nsGraphics::nsPMDModels::CPMDRenderer* m_rukaPmdR = nullptr;
		nsGraphics::ns2D::CSprite* sprite = nullptr;
		nsGraphics::nsFBXModels::CFBXRenderer* m_boxFbxR = nullptr;
	};
}

