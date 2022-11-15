#include "AnimationClip.h"
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			CAnimationClip::~CAnimationClip()
			{
				Terminate();
				return;
			}

			void CAnimationClip::Terminate()
			{
				Release();
				return;
			}

			void CAnimationClip::Release()
			{

				return;
			}


			bool CAnimationClip::Init()
			{

				return true;
			}





		}
	}
}