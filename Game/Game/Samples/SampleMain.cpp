#include "SampleMain.h"
#include "SkeltalAnimationSample.h"
#include "SpriteSample.h"
#include "FontSample.h"
#include "PhysicsSample.h"
#include "TriggerSample.h"
#include "CharacterControllerSample.h"
#include "AnimationEventSample.h"
#include "GetBoneMatrixSample.h"
#include "EffectSample.h"
#include "SoundSample.h"
#include "NavMeshSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		bool CSampleMain::Start()
		{
			constexpr int kSampleIdx = 10;

			switch (kSampleIdx)
			{
			case 0:
				m_sampleGO = NewGO<CSkeltalAnimationSample>(EnGOPriority::enMid, "SkeltalAnimationSample");
				break;
			case 1:
				m_sampleGO = NewGO<CSpriteSample>(EnGOPriority::enMid, "SpriteSample");
				break;
			case 2:
				m_sampleGO = NewGO<CFontSample>(EnGOPriority::enMid, "FontSample");
				break;
			case 3:
				m_sampleGO = NewGO<CPhysicsSample>(EnGOPriority::enMid, "PhysicsSample");
				break;
			case 4:
				m_sampleGO = NewGO<CTriggerSample>(EnGOPriority::enMid, "TriggerSample");
				break;
			case 5:
				m_sampleGO = NewGO<CCharacterControllerSample>(EnGOPriority::enMid, "CharacterControllerSample");
				break;
			case 6:
				m_sampleGO = NewGO<CAnimationEventSample>(EnGOPriority::enMid, "AnimationEventSample");
				break;
			case 7:
				m_sampleGO = NewGO<CGetBoneMatrixSample>(EnGOPriority::enMid, "GetBoneMatrixSample");
				break;
			case 8:
				m_sampleGO = NewGO<CEffectSample>(EnGOPriority::enMid, "EffectSample");
				break;
			case 9:
				m_sampleGO = NewGO<CSoundSample>(EnGOPriority::enMid, "SoundSample");
				break;
			case 10:
				m_sampleGO = NewGO<CNavMeshSample>(EnGOPriority::enMid, "NavMeshSample");
				break;
			default:
				m_sampleGO = NewGO<CSkeltalAnimationSample>(EnGOPriority::enMid, "SkeltalAnimationSample");
				break;
			}

			return true;
		}

		void CSampleMain::OnDestroy()
		{
			DeleteGO(m_sampleGO);

			return;
		}

		void CSampleMain::Update(float deltaTime)
		{
			if (Keyboard()->IsTrigger(EnKeyButton::enEscape))
			{
				DeleteGO(this);
				ExitGame();
			}
			return;
		}
	}
}