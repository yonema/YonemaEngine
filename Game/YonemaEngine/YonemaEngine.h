#pragma once
#include "Time/GameTime.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		class CGraphicsEngine; 
		namespace nsPMDModels
		{
			class CPMDRenderer;
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
			}
		}
		inline static CYonemaEngine* GetInstance()
		{
			return m_instance;
		}


		bool Init();

		void Update();

		constexpr float GetDeltaTime() const noexcept
		{
			return m_gameTime.GetDeltaTime();
		}

	private:

		void Terminate();

	private:
		static CYonemaEngine* m_instance;
		nsGraphics::CGraphicsEngine* m_graphicsEngine = nullptr;
		nsTime::CGameTime m_gameTime;
		nsGraphics::nsPMDModels::CPMDRenderer* m_mikuPmdR = nullptr;
		nsGraphics::nsPMDModels::CPMDRenderer* m_mikuMetalPmdR = nullptr;
		nsGraphics::nsPMDModels::CPMDRenderer* m_rukaPmdR = nullptr;
	};
}

