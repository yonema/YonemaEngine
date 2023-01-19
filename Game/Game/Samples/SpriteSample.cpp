#include "SpriteSample.h"
namespace nsAWA
{
	namespace nsSamples
	{
		const char* CSpriteSample::m_kSpriteFilePath = "Assets/Images/Sapmles/200x200PNG.png";

		bool CSpriteSample::Start()
		{
			SSpriteInitData spriteInitData;
			spriteInitData.filePath = m_kSpriteFilePath;
			spriteInitData.spriteSize = { 300.0f,300.0f };
			spriteInitData.alphaBlendMode = EnAlphaBlendMode::enTrans;
			m_spriteRenderer = NewGO<CSpriteRenderer>();
			m_spriteRenderer->Init(spriteInitData);
			m_spriteRenderer->SetPosition({ 0.0f,0.0f });
			m_spriteRenderer->SetPivot({ 1.0f,1.0f });


			return true;
		}

		void CSpriteSample::OnDestroy()
		{
			DeleteGO(m_spriteRenderer);
			return;
		}

		void CSpriteSample::Update(float deltaTime)
		{
			// スプライト回転
			auto rotTmp = m_spriteRenderer->GetRotation();
			rotTmp.AddRotationZDeg(50.0f * deltaTime);
			m_spriteRenderer->SetRotation(rotTmp);

			// 乗算カラー
			static bool hoge = true;
			auto mulCol = m_spriteRenderer->GetMulColor();
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
			m_spriteRenderer->SetMulColor(mulCol);


			return;
		}
	}
}