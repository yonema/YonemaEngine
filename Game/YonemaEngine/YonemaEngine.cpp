#include "YonemaEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "Input/InputManager.h"
#include "Physics/PhysicsEngine.h"
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
		if (m_graphicsEngine->Init() != true)
		{
			return false;
		}
		m_gameObjectManager = nsGameObject::CGameObjectManager::CreateInstance();
		m_inputManager = new nsInput::CInputManager();
		m_physicsWorld = nsPhysics::CPhysicsEngine::CreateInstance();

		//NewGO<nsAWA::CGame>(EnGOPriority::enMid, "AWAGame");
		NewGO<nsAWA::nsSamples::CSampleMain> ("SampleMain");

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

		// GameObjectManagerを先に消して、ゲームオブジェクトを全て破棄しておく。
		// ゲームオブジェクトのOnDestroyで各種エンジンを使用する処理を書いている場合があるため。
		nsGameObject::CGameObjectManager::DeleteInstance();

		nsPhysics::CPhysicsEngine::DeleteInstance();
		if (m_inputManager)
		{
			delete m_inputManager;
		}
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

		m_physicsWorld->Update(deltaTime);

		m_graphicsEngine->Update();

		// Draw処理

		m_graphicsEngine->ExecuteDraw();

		m_gameTime.EndTimeMeasurement();

		return;
	}
}
