#include "SpriteSample.h"
namespace nsAWA
{
	namespace nsScenes
	{
		namespace nsSample
		{
			const char* CSpriteSample::m_kSpriteFilePath = "Assets/Images/200x200PNG.png";

			bool CSpriteSample::Start()
			{
				SSpriteInitData spriteInitData;
				spriteInitData.filePath = m_kSpriteFilePath;
				spriteInitData.spriteSize = { 300.0f,300.0f };
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

				return;
			}
		}
	}
}