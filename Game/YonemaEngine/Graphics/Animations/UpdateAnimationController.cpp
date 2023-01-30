#include "UpdateAnimationController.h"
#include "UpdateAnimationManager.h"
#include "../GraphicsEngine.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			CUpdateAnimationController::CUpdateAnimationController()
			{
				CUpdateAnimationManager::GetInstance()->AddUpdateAnimController(this);

				return;
			}

			CUpdateAnimationController::~CUpdateAnimationController()
			{
				CUpdateAnimationManager::GetInstance()->RemoveUpdateAnimController(this);

				return;
			}

			void CUpdateAnimationController::Update(const nsMath::CVector3& pos)
			{
				if (m_distanceToReducingUpdate < 0.0f)
				{
					Updatable();
					return;
				}


				const auto toCamPos =
					CGraphicsEngine::GetInstance()->GetMainCamera()->GetPosition() - pos;
				const float toCamDist = toCamPos.Length();

				if (toCamDist > m_distanceToReducingUpdate)
				{
					m_isReducingUpdate = true;
				}
				else
				{
					m_isReducingUpdate = false;
					Updatable();
				}


				return;
			}



		}
	}
}
