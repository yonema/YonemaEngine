#include "Animator.h"
#include "../Models/AssimpCommon.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			SAnimationInitData::SAnimationInitData(
				unsigned int numAnimations,
				const char* animationFilePaths[]
			)
				:numAnimations(numAnimations)
			{
				animationFilePathArray.reserve(numAnimations);
				for (unsigned int animIdx = 0; animIdx < numAnimations; animIdx++)
				{
					animationFilePathArray.emplace_back(animationFilePaths[animIdx]);
				}
				return;
			}


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

			bool CAnimator::Init(const SAnimationInitData& animInitData, CSkelton* pSkelton)
			{
				bool res = InitAnimationClips(animInitData, pSkelton);

				return res;
			}

			bool CAnimator::InitAnimationClips(const SAnimationInitData& animInitData, CSkelton* pSkelton)
			{
				bool res = false;
				m_animationClips.reserve(animInitData.numAnimations);

				for (unsigned int animIdx = 0; animIdx < animInitData.numAnimations; animIdx++)
				{
					m_animationClips.emplace_back(new CAnimationClip());
					res = m_animationClips[animIdx]->Init(
						animInitData.animationFilePathArray[animIdx], pSkelton);

					if (res != true)
					{
						return false;
					}
				}

				return true;
			}

			void CAnimator::PlayAnimation(unsigned int animIdx) noexcept
			{
				if (m_animationIndex == animIdx/* && m_isPlaying == true*/)
				{
					// 現在のアニメーションと指定されたアニメーションが一緒なら変更無し。
					return;
				}

				PlayAnimationFromBeginning(animIdx);

				return;
			}

			void CAnimator::PlayAnimationFromBeginning(unsigned int animIdx) noexcept
			{
				if (animIdx >= static_cast<unsigned int>(m_animationClips.size()))
				{
					// 指定されたアニメーションインデックスが、アニメーション数を超えている。
					return;
				}
				m_animationClips[m_animationIndex]->ResetAnimationParam();
				m_animationIndex = animIdx;
				m_animationTimer = 0.0f;
				m_isPlaying = true;

				return;
			}


			void CAnimator::CalcAndGetAnimatedBoneTransforms(
				std::vector<nsMath::CMatrix>* pMTransforms) noexcept
			{
				m_animationClips[m_animationIndex]->CalcAndGetAnimatedBoneTransforms(
					m_animationTimer, pMTransforms, 0, m_isLoop);

				m_isPlaying = !m_animationClips[m_animationIndex]->IsPlayedAnimationToEnd();

				return;
			}

			void CAnimator::UpdateAnimation(float deltaTime) noexcept
			{
				if (m_isPlaying != true)
				{
					return;
				}

				m_animationTimer += deltaTime * m_animationSpeed;

				return;
			}


		}
	}
}