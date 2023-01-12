#pragma once
#include "AnimationClip.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CSkelton;
		}
	}
}



namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			struct SAnimationInitData : private nsUtils::SNoncopyable
			{
				constexpr SAnimationInitData() = default;

				SAnimationInitData(
					unsigned int numAnimations,
					const char* animationFilePaths[]
				);

				void Init(
					unsigned int numAnims,
					const char* animFilePaths[]
				);

				unsigned int numAnimations = 0;
				std::vector<const char*> animationFilePathArray = {};
			};

			class CAnimator : nsUtils::SNoncopyable
			{
			public:
				constexpr CAnimator() = default;
				~CAnimator();

				bool Init(
					const SAnimationInitData& animInitData,
					CSkelton* pSkelton,
					bool loadingAsynchronous
				);

				void Release();

				void PlayAnimation(unsigned int animIdx) noexcept;

				void PlayAnimationFromBeginning(unsigned int animIdx) noexcept;

				void CalcAndGetAnimatedBoneTransforms(
					std::vector<nsMath::CMatrix>* pMTransforms) noexcept;

				void UpdateAnimation(float deltaTime) noexcept;

				constexpr void SetAnimationSpeed(float animSpeed) noexcept
				{
					animSpeed >= 0.0f ?
						m_animationSpeed = animSpeed : m_animationSpeed = m_animationSpeed;
				}

				constexpr bool IsPlaying() const noexcept
				{
					return m_isPlaying;
				}

				constexpr void SetIsLoop(bool isLoop)noexcept
				{
					m_isLoop = isLoop;
				}

				constexpr bool IsPlayedAnimationToEnd() const noexcept
				{
					return m_animationClips[m_animationIndex]->IsPlayedAnimationToEnd();
				}

				inline void ReserveAnimationEventFuncArray(unsigned int animIdx, unsigned int size)
				{
					if (animIdx >= m_animationClips.size())
					{
						return;
					}
					m_animationClips[animIdx]->ReserveAnimationEventFuncArray(size);
				}

				inline void AddAnimationEventFunc(
					unsigned int animIdx,
					const std::function<void(void)>& animationEventFunc)
				{
					if (animIdx >= m_animationClips.size())
					{
						return;
					}
					m_animationClips[animIdx]->AddAnimationEventFunc(animationEventFunc);
				}

				inline bool IsLoaded() const noexcept
				{
					if (m_animationClips.empty())
					{
						return true;
					}

					for (const auto* animClip : m_animationClips)
					{
						if (animClip->IsLoaded() != true)
						{
							return false;
						}
					}

					return true;
				}

			private:

				void Terminate();

				bool InitAnimationClips(
					const SAnimationInitData& animInitData,
					CSkelton* pSkelton,
					bool loadingAsynchronous
				);


			private:
				std::vector<CAnimationClip*> m_animationClips = {};
				unsigned int m_animationIndex = 0;
				float m_animationTimer = 0.0f;
				bool m_isPlaying = true;	// çÏê¨ÇµÇΩèuä‘Ç©ÇÁçƒê∂äJén
				float m_animationSpeed = 1.0f;
				bool m_isLoop = true;
				CSkelton* m_pSkelton = nullptr;
			};

		}
	}
}