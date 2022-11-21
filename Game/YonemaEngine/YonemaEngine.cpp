#include "YonemaEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "Input/InputManager.h"
#include "DebugSystem/DisplayFPS.h"
#include "../Game/Game.h"
#include "../Game/Samples/SampleMain.h"

namespace nsYMEngine
{
	CYonemaEngine* CYonemaEngine::m_instance = nullptr;

	CYonemaEngine::~CYonemaEngine()
	{
		Terminate();
		m_instance = nullptr;
		return;
	}

	bool CYonemaEngine::Init()
	{
		m_graphicsEngine = nsGraphics::CGraphicsEngine::CreateInstance();
		m_gameObjectManager = nsGameObject::CGameObjectManager::CreateInstance();

		if (m_graphicsEngine->Init() != true)
		{
			return false;
		}

		m_inputManager = new nsInput::CInputManager();

		//NewGO<nsAWA::CGame>(EnGOPriority::enMid, "AWAGame");
		NewGO<nsAWA::nsScenes::nsSample::CSampleMain> ("SampleMain");

#ifdef _DEBUG
		m_displayFPS = NewGO<nsDebugSystem::CDisplayFPS>("DisplayFPS");
#endif // _DEBUG


		return true;
	}

	void CYonemaEngine::Terminate()
	{
#ifdef _DEBUG
		DeleteGO(m_displayFPS);
#endif // _DEBUG

		if (m_inputManager)
		{
			delete m_inputManager;
		}
		nsGameObject::CGameObjectManager::DeleteInstance();
		m_gameObjectManager = nullptr;
		nsGraphics::CGraphicsEngine::DeleteInstance();
		m_graphicsEngine = nullptr;

		return;
	}

	void CYonemaEngine::Update()
	{
		m_gameTime.StartTimeMeasurement();

		const float deltaTime = GetDeltaTime();

		m_inputManager->Update(deltaTime);

		m_gameObjectManager->Update(deltaTime);

		m_graphicsEngine->Update();

		// Drawˆ—

		m_graphicsEngine->ExecuteDraw();



		m_gameTime.EndTimeMeasurement();


		return;
	}
}
