#include "SampleMain.h"
#include "SkeltalAnimationSample.h"
#include "SpriteSample.h"
#include "FontSample.h"

namespace nsAWA
{
	namespace nsScenes
	{
		namespace nsSample
		{
			bool CSampleMain::Start()
			{
				constexpr int kSampleIdx = 2;
				
				switch (kSampleIdx)
				{
				case 0:
					NewGO<CSkeltalAnimationSample>(EnGOPriority::enMid, "SkeltalAnimationSample");
					break;
				case 1:
					NewGO<CSpriteSample>(EnGOPriority::enMid, "SpriteSample");
					break;
				case 2:
					NewGO<CFontSample>(EnGOPriority::enMid, "FontSample");
					break;
				default:
					NewGO<CSkeltalAnimationSample>(EnGOPriority::enMid, "SkeltalAnimationSample");
					break;
				}

				return true;
			}

			void CSampleMain::OnDestroy()
			{

				return;
			}

			void CSampleMain::Update(float deltaTime)
			{
				if (Keyboard()->IsTrigger(EnKeyButton::enEscape))
				{
					ExitGame();
				}
				return;
			}
		}
	}
}