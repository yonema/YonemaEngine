#include "SpriteSample.h"
namespace nsAWA
{
	namespace nsSamples
	{
		const char* CSpriteSample::m_kSpriteFilePath[m_kNumSprites] =
		{
			"Assets/Images/Sapmles/200x200PNG.png",
			"Assets/Images/Sapmles/256x256PNG.png"
		};

		bool CSpriteSample::Start()
		{
			SSpriteInitData spriteInitData[m_kNumSprites];
			spriteInitData[0].filePath = m_kSpriteFilePath[0];
			spriteInitData[0].spriteSize = { 200.0f, 200.0f };
			spriteInitData[0].priority = EnRendererPriority::enMax;

			spriteInitData[1].filePath = m_kSpriteFilePath[1];
			spriteInitData[1].spriteSize = { 256.0f, 256.0f };

			for (unsigned int i = 0; i < m_kNumSprites; i++)
			{
				spriteInitData[i].alphaBlendMode = EnAlphaBlendMode::enTrans;
				m_spriteRenderer[i] = NewGO<CSpriteRenderer>();
				m_spriteRenderer[i]->Init(spriteInitData[i]);
			}

			m_spriteRenderer[0]->SetPosition({ -50.0f,0.0f });
			m_spriteRenderer[1]->SetPosition({ 50.0f,0.0f });


			return true;
		}

		void CSpriteSample::OnDestroy()
		{
			for (unsigned int i = 0; i < m_kNumSprites; i++)
			{
				DeleteGO(m_spriteRenderer[i]);
			}
			return;
		}

		void CSpriteSample::Update(float deltaTime)
		{

			if (Keyboard()->IsTrigger(EnKeyButton::en0))
			{
				m_spriteRenderer[0]->SetDrawingFlag(!m_spriteRenderer[0]->IsDrawingFlag());
			}
			if (Keyboard()->IsTrigger(EnKeyButton::en1))
			{
				m_spriteRenderer[1]->SetDrawingFlag(!m_spriteRenderer[1]->IsDrawingFlag());
			}

			for (unsigned int i = 0; i < m_kNumSprites; i++)
			{
				// スプライト回転
				auto rotTmp = m_spriteRenderer[i]->GetRotation();
				rotTmp.AddRotationZDeg(50.0f * deltaTime);
				m_spriteRenderer[i]->SetRotation(rotTmp);

				// 乗算カラー
				static bool hoge = true;
				auto mulCol = m_spriteRenderer[i]->GetMulColor();
				if (hoge)
				{
					mulCol.a -= 0.5f * deltaTime;
					if (mulCol.a <= 0.0f)
					{
						mulCol.a = 0.0f;
						hoge = false;
					}
				}
				else
				{
					mulCol.a += 0.5f * deltaTime;
					if (mulCol.a >= 1.0f)
					{
						mulCol.a = 1.0f;
						hoge = true;
					}
				}
				//m_spriteRenderer[i]->SetMulColor(mulCol);
			}



			return;
		}
	}
}