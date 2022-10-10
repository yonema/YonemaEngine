#include "YonemaEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "Graphics/PMDModels/PMDRenderer.h"

namespace nsYMEngine
{
	CYonemaEngine* CYonemaEngine::m_instance = nullptr;

	CYonemaEngine::~CYonemaEngine()
	{
		Terminate();

		return;
	}

	bool CYonemaEngine::Init()
	{
		m_graphicsEngine = nsGraphics::CGraphicsEngine::CreateInstance();

		if (m_graphicsEngine->Init() != true)
		{
			return false;
		}

		m_mikuPmdR = new nsGraphics::nsPMDModels::CPMDRenderer("Assets/Models/Samples/初音ミク.pmd");
		m_mikuMetalPmdR = new nsGraphics::nsPMDModels::CPMDRenderer("Assets/Models/Samples/初音ミクmetal.pmd");
		m_rukaPmdR = new nsGraphics::nsPMDModels::CPMDRenderer("Assets/Models/Samples/巡音ルカ.pmd");

		constexpr float distance = 15.0f;
		m_mikuPmdR->DebugSetPosition(distance);
		m_mikuMetalPmdR->DebugSetPosition(0.0f);
		m_rukaPmdR->DebugSetPosition(-distance);

		return true;
	}

	void CYonemaEngine::Terminate()
	{
		delete m_rukaPmdR;
		delete m_mikuMetalPmdR;
		delete m_mikuPmdR;

		nsGraphics::CGraphicsEngine::DeleteInstance();

		return;
	}

	void CYonemaEngine::Update()
	{
		m_graphicsEngine->BeginDraw();

		m_mikuPmdR->Update();
		m_mikuMetalPmdR->Update();
		m_rukaPmdR->Update();

		m_graphicsEngine->Update();

		m_mikuPmdR->Draw();
		m_mikuMetalPmdR->Draw();
		m_rukaPmdR->Draw();

		m_graphicsEngine->EndDraw();


		return;
	}
}
