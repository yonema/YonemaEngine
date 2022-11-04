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
		m_unityChanMR = NewGO<CModelRenderer>(EnGOPriority::enMid, "TestModel");
		auto hogeSR = NewGO<CSpriteRenderer>(EnGOPriority::enMid, "TestSprite");

		SModelInitData modelInitData;
		modelInitData.modelFilePath = "Assets/Models/Samples/初音ミク.pmd";
		modelInitData.animFilePath = "Assets/Animations/Samples/charge.vmd";
		mikuMR->SetPosition({ 15.0f,0.0f,0.0f });
		mikuMR->Init(modelInitData);

		modelInitData.modelFilePath = "Assets/Models/Samples/初音ミクmetal.pmd";
		modelInitData.animFilePath = "Assets/Animations/Samples/pose.vmd";
		mikuMetalMR->SetPosition({ -15.0f,0.0f,0.0f });
		mikuMetalMR->Init(modelInitData);

		modelInitData.modelFilePath = "Assets/Models/unitychan.fbx";
		modelInitData.animFilePath = nullptr;
		modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);
		m_unityChanMR->SetPosition({ 0.0f,0.0f,0.0f });
		m_unityChanMR->SetScale(10.0f);
		m_unityChanMR->Init(modelInitData);

		SSpriteInitData spriteInitData;
		spriteInitData.filePath = "Assets/Images/200x200PNG.png";
		spriteInitData.spriteSize = { 200.0f,200.0f };
		spriteInitData.alphaBlendMode = nsGraphics::ns2D::EnAlphaBlendMode::enTrans;
		hogeSR->SetPosition({ 200.0f,200.0f });
		hogeSR->Init(spriteInitData);

		return true;
	}

	void CGame::Update(float deltaTime)
	{
		m_debugNum++;

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

		auto sprite = FindGO<CSpriteRenderer>("TestSprite");
		if (sprite)
		{
			auto rot = sprite->GetRotation();
			rot.AddRotationZ(10.0f * deltaTime);
			sprite->SetRotation(rot);
		}

		static int unityOrVrmOrBox = 0;

		if (m_debugNum >= 360)
		{
			if (m_unityChanMR)
			{
				DeleteGO(m_unityChanMR);
				m_unityChanMR = nullptr;
			}
			else
			{
				unityOrVrmOrBox++;
				if (unityOrVrmOrBox > 2)
				{
					unityOrVrmOrBox = 0;
				}

				m_unityChanMR = NewGO<CModelRenderer>(EnGOPriority::enMid, "TestModel");
				SModelInitData modelInitData;
				modelInitData.vertexBias.SetRotationX(nsMath::YM_PIDIV2);
				if (unityOrVrmOrBox == 0)
				{
					modelInitData.modelFilePath = "Assets/Models/unitychan.fbx";
				}
				else if (unityOrVrmOrBox == 1)
				{
					modelInitData.modelFilePath = "Assets/Models/hoge.fbx";
				}
				else
				{
					modelInitData.modelFilePath = "Assets/Models/SampleBox.fbx";
				}

				m_unityChanMR->SetPosition({ 0.0f,0.0f,0.0f });
				m_unityChanMR->SetScale(10.0f);
				m_unityChanMR->Init(modelInitData);
			}
			m_debugNum = 0;
			//CYonemaEngine::GetInstance()->ExitGame();
		}



		return;
	}

	void CGame::OnDestroy()
	{
		m_debugNum = 0;

		if (m_unityChanMR)
		{
			DeleteGO(m_unityChanMR);
		}

		return;
	}


}
