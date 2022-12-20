#include "SampleMain.h"
#include "SkeltalAnimationSample.h"
#include "SpriteSample.h"
#include "FontSample.h"
#include "PhysicsSample.h"
#include "TriggerSample.h"
#include "CharacterControllerSample.h"
#include "AnimationEventSample.h"
#include "GetBoneMatrixSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CSampleMain::Start()
		{
			constexpr int kSampleIdx = 5;

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
			case 3:
				NewGO<CPhysicsSample>(EnGOPriority::enMid, "PhysicsSample");
				break;
			case 4:
				NewGO<CTriggerSample>(EnGOPriority::enMid, "TriggerSample");
				break;
			case 5:
				NewGO<CCharacterControllerSample>(EnGOPriority::enMid, "CharacterControllerSample");
				break;
			case 6:
				NewGO<CAnimationEventSample>(EnGOPriority::enMid, "AnimationEventSample");
				break;
			case 7:
				NewGO<CGetBoneMatrixSample>(EnGOPriority::enMid, "GetBoneMatrixSample");
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