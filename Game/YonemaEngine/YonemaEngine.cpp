#include "YonemaEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "../Game/Game.h"
#include "Graphics/PMDModels/PMDRenderer.h"
#include "Graphics/FBXModels/FBXRenderer.h"
#include "Graphics/2D/Sprite.h"

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

		NewGO<nsAWA::CGame>(EnGOPriority::enMid, "AWAGame");

		nsMath::CMatrix mat = nsMath::CMatrix::Identity();

		return true;
	}

	void CYonemaEngine::Terminate()
	{
		nsGameObject::CGameObjectManager::DeleteInstance();
		m_gameObjectManager = nullptr;
		nsGraphics::CGraphicsEngine::DeleteInstance();
		m_graphicsEngine = nullptr;

		return;
	}

	void CYonemaEngine::Update()
	{
		m_gameTime.StartTimeMeasurement();

		m_gameObjectManager->Update(GetDeltaTime());

		m_graphicsEngine->Update();

		// Draw処理

		// 描画開始処理。更新処理より後、描画処理より前に呼ぶこと。
		m_graphicsEngine->BeginDraw();

		m_graphicsEngine->DrawToMainRenderTarget();

		m_graphicsEngine->DrawWithSimplePostEffect();

		m_graphicsEngine->Draw2D();

		m_graphicsEngine->EndDraw();

		m_gameTime.EndTimeMeasurement();


		return;
	}
}
