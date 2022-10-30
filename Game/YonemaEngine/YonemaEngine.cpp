#include "YonemaEngine.h"
#include "Graphics/GraphicsEngine.h"
#include "Graphics/PMDModels/PMDRenderer.h"
#include "Graphics/2D/Sprite.h"

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

		m_mikuPmdR = new nsGraphics::nsPMDModels::CPMDRenderer(
			"Assets/Models/Samples/初音ミク.pmd",
			//"Assets/Animations/Samples/squat.vmd"
			"Assets/Animations/Samples/charge.vmd"
		);
		m_mikuMetalPmdR = new nsGraphics::nsPMDModels::CPMDRenderer(
			"Assets/Models/Samples/初音ミクmetal.pmd", "Assets/Animations/Samples/pose.vmd");
		m_rukaPmdR = new nsGraphics::nsPMDModels::CPMDRenderer("Assets/Models/Samples/巡音ルカ.pmd");

		constexpr float distance = 15.0f;
		m_mikuPmdR->DebugSetPosition(distance);
		m_mikuMetalPmdR->DebugSetPosition(0.0f);
		m_rukaPmdR->DebugSetPosition(-distance);

		nsMath::CMatrix mat = nsMath::CMatrix::Identity();

		sprite = new nsGraphics::ns2D::CSprite();
		nsGraphics::ns2D::SSpriteInitData initData;
		initData.filePath = "Assets/Images/200x200PNG.png";
		initData.spriteSize = { 200.0f,200.0f };
		initData.alphaBlendMode = nsGraphics::ns2D::EnAlphaBlendMode::enTrans;
		sprite->Init(initData);


		return true;
	}

	void CYonemaEngine::Terminate()
	{
		if (sprite)
		{
			sprite->Release();
			delete sprite;
		}

		if (m_rukaPmdR)
		{
			delete m_rukaPmdR;
		}
		if (m_mikuMetalPmdR)
		{
			delete m_mikuMetalPmdR;
		}
		if (m_mikuPmdR)
		{
			delete m_mikuPmdR;
		}

		nsGraphics::CGraphicsEngine::DeleteInstance();

		return;
	}

	void CYonemaEngine::Update()
	{
		m_gameTime.StartTimeMeasurement();
		// アップデート中の、モデルの破棄と生成のテスト。

		static int debugCount = 0;
		debugCount++;
		if (debugCount > 120 && debugCount <= 240)
		{
			if (m_mikuMetalPmdR)
			{
				delete m_mikuMetalPmdR;
				m_mikuMetalPmdR = nullptr;
			}
		}
		else if (debugCount > 240)
		{
			if (m_mikuMetalPmdR == nullptr)
			{
				m_mikuMetalPmdR = new nsGraphics::nsPMDModels::CPMDRenderer("Assets/Models/Samples/初音ミクmetal.pmd", "Assets/Animations/Samples/pose.vmd");
				debugCount = 0;
			}
		}

		// Update処理
		m_mikuPmdR->Update();
		if (m_mikuMetalPmdR)
		{
			m_mikuMetalPmdR->Update();
		}
		m_rukaPmdR->Update();

		static nsMath::CVector3 pos = nsMath::CVector3::Zero();
		static nsMath::CQuaternion rot = nsMath::CQuaternion::Identity();
		static nsMath::CVector3 scale = nsMath::CVector3::One();
		static int spriteCounter = 0;

		constexpr float spriteMoveSpeed = 2.0f;
		constexpr float spriteRotateSpeed = 1.0f;
		constexpr float spriteScaleSpeed = 0.002f;
		constexpr int spriteMaxCounter = 720;

		if (spriteCounter <= spriteMaxCounter / 2)
		{
			pos.x += spriteMoveSpeed;
			scale.Add({ spriteScaleSpeed,spriteScaleSpeed,spriteScaleSpeed });
			rot.AddRotationZDeg(spriteRotateSpeed);
		}
		else if (spriteCounter <= spriteMaxCounter)
		{
			pos.x -= spriteMoveSpeed;
			scale.Subtract({ spriteScaleSpeed,spriteScaleSpeed,spriteScaleSpeed });
			rot.AddRotationZDeg(-spriteRotateSpeed);
		}
		else
		{
			spriteCounter = 0;
			if (sprite)
			{
				delete sprite;
				sprite = nullptr;
			}
			else
			{
				sprite = new nsGraphics::ns2D::CSprite();
				nsGraphics::ns2D::SSpriteInitData initData;
				initData.filePath = "Assets/Images/200x200PNG.png";
				initData.spriteSize = { 200.0f,200.0f };
				initData.alphaBlendMode = nsGraphics::ns2D::EnAlphaBlendMode::enTrans;
				sprite->Init(initData);
			}
		}
		spriteCounter++;

		if (sprite)
		{
			sprite->UpdateWorldMatrix(pos, rot, scale, { 0.5f,0.5f });
			float value = static_cast<float>(spriteCounter) / spriteMaxCounter;
			sprite->SetMulColor({ 1.0f,1.0f,1.0f,value });
		}

		m_graphicsEngine->Update();

		// Draw処理

		// 描画開始処理。更新処理より後、描画処理より前に呼ぶこと。
		m_graphicsEngine->BeginDraw();

		m_graphicsEngine->DrawToMainRenderTarget();

		m_mikuPmdR->Draw();
		if (m_mikuMetalPmdR)
		{
			m_mikuMetalPmdR->Draw();
		}
		m_rukaPmdR->Draw();

		m_graphicsEngine->DrawWithSimplePostEffect();

		m_graphicsEngine->Draw2D();

		if (sprite)
		{
			sprite->Draw();
		}

		m_graphicsEngine->EndDraw();

		m_gameTime.EndTimeMeasurement();


		return;
	}
}
