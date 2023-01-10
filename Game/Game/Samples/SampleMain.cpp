#include "SampleMain.h"
#include "ModelRendererSample.h"
#include "SkeltalAnimationSample.h"
#include "AnimationEventSample.h"
#include "GetBoneMatrixSample.h"
#include "SpriteSample.h"
#include "FontSample.h"
#include "EffectSample.h"
#include "SoundSample.h"
#include "NavMeshSample.h"
#include "PhysicsSample.h"
#include "TriggerSample.h"
#include "CharacterControllerSample.h"

namespace nsAWA
{
	namespace nsSamples
	{
		namespace
		{
			enum class EnSampleIdx
			{
				ModelRenderer,
				SkeltalAnimation,
				AnimationEvent,
				GetBoneMatrix,
				Sprite,
				Font,
				Effect,
				Sound,
				NavMesh,
				Physics,
				Trigger,
				CharacterController,
			};

			constexpr EnSampleIdx g_kSampleIdx = EnSampleIdx::ModelRenderer;
		}

		bool CSampleMain::Start()
		{
			switch (g_kSampleIdx)
			{
			case EnSampleIdx::ModelRenderer:
				m_sampleGO = NewGO<CModelRendererSample>("ModelRendererSample");
				break;
			case EnSampleIdx::SkeltalAnimation:
				m_sampleGO = NewGO<CSkeltalAnimationSample>("SkeltalAnimationSample");
				break;
			case EnSampleIdx::AnimationEvent:
				m_sampleGO = NewGO<CAnimationEventSample>("AnimationEventSample");
				break;
			case EnSampleIdx::GetBoneMatrix:
				m_sampleGO = NewGO<CGetBoneMatrixSample>("GetBoneMatrixSample");
				break;
			case EnSampleIdx::Sprite:
				m_sampleGO = NewGO<CSpriteSample>("SpriteSample");
				break;
			case EnSampleIdx::Font:
				m_sampleGO = NewGO<CFontSample>("FontSample");
				break;
			case EnSampleIdx::Effect:
				m_sampleGO = NewGO<CEffectSample>("EffectSample");
				break;
			case EnSampleIdx::Sound:
				m_sampleGO = NewGO<CSoundSample>("SoundSample");
				break;
			case EnSampleIdx::NavMesh:
				m_sampleGO = NewGO<CNavMeshSample>("NavMeshSample");
				break;
			case EnSampleIdx::Physics:
				m_sampleGO = NewGO<CPhysicsSample>("PhysicsSample");
				break;
			case EnSampleIdx::Trigger:
				m_sampleGO = NewGO<CTriggerSample>("TriggerSample");
				break;
			case EnSampleIdx::CharacterController:
				m_sampleGO = NewGO<CCharacterControllerSample>("CharacterControllerSample");
				break;
			default:
				m_sampleGO = NewGO<CModelRendererSample>("ModelRendererSample");
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