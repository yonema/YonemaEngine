#include "Animator.h"
#include "../Models/AssimpCommon.h"
#include "../../Thread/LoadModelThread.h"
#include "../../Memory/ResourceBankTable.h"

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

			void SAnimationInitData::Init(
				unsigned int numAnims,
				const char* const animFilePaths[]
			)
			{
				numAnimations = numAnims;

				animationFilePathArray.clear();
				animationFilePathArray.reserve(numAnimations);
				for (unsigned int animIdx = 0; animIdx < numAnimations; animIdx++)
				{
					animationFilePathArray.emplace_back(animFilePaths[animIdx]);
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
				for (auto& animClip : m_animationClips)
				{
					if (animClip == nullptr)
					{
						continue;
					}

					if (animClip->IsShared())
					{
						animClip = nullptr;
						continue;
					}

					delete animClip;
					animClip = nullptr;
				}

				m_animationClips.clear();

				return;
			}

			bool CAnimator::Init(
				const SAnimationInitData& animInitData, 
				CSkelton* pSkelton, 
				bool loadingAsynchronous,
				bool regiseterAnimBank
			)
			{
				m_pSkelton = pSkelton;
				bool res = InitAnimationClips(
					animInitData, pSkelton, loadingAsynchronous, regiseterAnimBank);

				return res;
			}

			bool CAnimator::InitAnimationClips(
				const SAnimationInitData& animInitData,
				CSkelton* pSkelton,
				bool loadingAsynchronous,
				bool regiseterAnimBank
			)
			{
				bool res = false;
				m_animationClips.reserve(animInitData.numAnimations);

				for (unsigned int animIdx = 0; animIdx < animInitData.numAnimations; animIdx++)
				{
					const auto* animFilePath = animInitData.animationFilePathArray[animIdx];

					auto& animClipBank = 
						nsMemory::CResourceBankTable::GetInstance()->GetAnimationClipBank();
					auto* animClip = animClipBank.Get(animFilePath);

					if (animClip == nullptr)
					{
						// AnimBankに未登録のため、新規作成する。

						animClip = new CAnimationClip();

						// AnimationClipBankのRegisterはCAnimationClipの中で行う

						if (loadingAsynchronous)
						{
							nsThread::CLoadModelThread::GetInstance()->PushLoadModelProcess(
								nsThread::CLoadModelThread::EnLoadProcessType::enLoadAnim,
								nullptr,
								animClip,
								animFilePath,
								pSkelton,
								regiseterAnimBank
							);
							res = true;
						}
						else
						{
							res = animClip->Init(
								animInitData.animationFilePathArray[animIdx], regiseterAnimBank);
						}

					}
					else
					{
						// AnimBankに登録済み
						res = true;
					}

					m_animationClips.emplace_back(animClip);



					if (res != true)
					{
						std::wstring wstr = nsUtils::GetWideStringFromString(animFilePath);
						wstr.erase(wstr.end() - 1);
						wstr += L"のアニメーションのロードに失敗しました。";
						nsGameWindow::MessageBoxError(wstr.c_str());
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

			void CAnimator::PlayAnimationFromMiddle(unsigned int animIdx, float timer) noexcept
			{
				if (animIdx >= static_cast<unsigned int>(m_animationClips.size()))
				{
					// 指定されたアニメーションインデックスが、アニメーション数を超えている。
					return;
				}
				m_animationClips[m_animationIndex]->ResetAnimationParam();
				m_animationIndex = animIdx;
				m_animationTimer = timer;
				m_isPlaying = true;

				return;
			}



			void CAnimator::CalcAndGetAnimatedBoneTransforms(
				std::vector<nsMath::CMatrix>* pMTransforms) noexcept
			{
				m_animationClips[m_animationIndex]->CalcAndGetAnimatedBoneTransforms(
					m_animationTimer, pMTransforms,m_pSkelton, 0, m_isLoop);

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