#include "YonemaEnginePreCompile.h"
#include "Game.h"
#include "../YonemaEngine.h"

namespace nsAWA
{
	const char* const CGame::m_kGameObjectName = "Game";

	bool CGame::Start()
	{

		m_debugNum++;

		auto mikuMR = NewGO<CModelRenderer>(EnGOPriority::enMid, "TestModel");
		auto mikuMetalMR = NewGO<CModelRenderer>(EnGOPriority::enMid, "TestModel");
		auto unityChanMR = NewGO<CModelRenderer>(EnGOPriority::enMid, "TestModel");

		SModelInitData initData;
		initData.modelFilePath = "Assets/Models/Samples/初音ミク.pmd";
		initData.animFilePath = "Assets/Animations/Samples/charge.vmd";
		mikuMR->SetPosition({ 15.0f,0.0f,0.0f });
		mikuMR->Init(initData);

		initData.modelFilePath = "Assets/Models/Samples/初音ミクmetal.pmd";
		initData.animFilePath = "Assets/Animations/Samples/pose.vmd";
		mikuMetalMR->SetPosition({ -15.0f,0.0f,0.0f });
		mikuMetalMR->Init(initData);

		initData.modelFilePath = "Assets/Models/unitychan.fbx";
		initData.animFilePath = nullptr;
		unityChanMR->SetPosition({ 0.0f,0.0f,0.0f });
		unityChanMR->SetScale(10.0f);
		unityChanMR->Init(initData);



		int a = 0;

		return true;
	}

	void CGame::Update(float deltaTime)
	{
		m_debugNum++;

		int a = 0;

		QueryGOs<CModelRenderer>(
			"TestModel", 
			[&](CModelRenderer* modelRender)
			{
				auto rot = modelRender->GetRotation();
				rot.AddRotationYDeg(100.0f * deltaTime);
				modelRender->SetRotation(rot);
				return true;
			}
		);

		if (m_debugNum >= 1000)
		{
			//CYonemaEngine::GetInstance()->ExitGame();
		}



		return;
	}

	void CGame::OnDestroy()
	{
		m_debugNum = 0;
		int a = 0;

		return;
	}


}
