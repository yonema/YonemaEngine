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

			void CUpdateAnimationController::Update(float distance)
			{
				if (m_distanceToReducingUpdate < 0.0f)
				{
					Updatable();
					return;
				}


				if (distance > m_distanceToReducingUpdate)
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
