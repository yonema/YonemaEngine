#include "Animator.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			CAnimator::~CAnimator()
			{
				Terminate();
				return;
			}

			void CAnimator::Terminate()
			{
				Release();
				return;
			}

			void CAnimator::Release()
			{

				return;
			}

			bool CAnimator::Init(
				unsigned int numAnimations,
				const char* animationFilePaths[],
				CAnimationController* animationController
			)
			{

				return true;
			}





		}
	}
}