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

		m_sprite = new nsGraphics::ns2D::CSprite();
		nsGraphics::ns2D::SSpriteInitData initData;
		initData.filePath = "Assets/Images/200x200PNG.png";
		initData.spriteSize = { 200.0f,200.0f };
		initData.alphaBlendMode = nsGraphics::ns2D::EnAlphaBlendMode::enTrans;
		m_sprite->Init(initData);
		m_graphicsEngine->GetRendererTable()->RegistRenderer(
			nsGraphics::nsRenderers::CRendererTable::EnRendererType::enTransSprite, m_sprite);

		return true;
	}

	void CYonemaEngine::Terminate()
	{

		if (m_sprite)
		{
			m_sprite->Release();
			delete m_sprite;
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

		m_gameObjectManager->Update(GetDeltaTime());

		// アップデート中の、モデルの破棄と生成のテスト。

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
			if (m_sprite)
			{
				m_graphicsEngine->GetRendererTable()->RemoveRenderer(
					nsGraphics::nsRenderers::CRendererTable::EnRendererType::enTransSprite, m_sprite);
				delete m_sprite;
				m_sprite = nullptr;
			}
			else
			{
				m_sprite = new nsGraphics::ns2D::CSprite();
				nsGraphics::ns2D::SSpriteInitData initData;
				initData.filePath = "Assets/Images/200x200PNG.png";
				initData.spriteSize = { 200.0f,200.0f };
				initData.alphaBlendMode = nsGraphics::ns2D::EnAlphaBlendMode::enTrans;
				m_graphicsEngine->GetRendererTable()->RegistRenderer(
					nsGraphics::nsRenderers::CRendererTable::EnRendererType::enTransSprite, m_sprite);
				m_sprite->Init(initData);
			}
		}
		spriteCounter++;

		if (m_sprite)
		{
			m_sprite->UpdateWorldMatrix(pos, rot, scale, { 0.5f,0.5f });
			float value = static_cast<float>(spriteCounter) / spriteMaxCounter;
			m_sprite->SetMulColor({ 1.0f,1.0f,1.0f,value });
		}

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
