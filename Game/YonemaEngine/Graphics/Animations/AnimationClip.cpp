#include "AnimationClip.h"
#include "../Models/AssimpCommon.h"
#include "Skelton.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"
#include "../../Memory/ResourceBankTable.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			const std::string CAnimationClip::m_kAnimEventKeyNodeName = "AnimEventKey";


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
				m_isLoaded = false;
				if (m_importer)
				{
					m_importer->FreeScene();
					delete m_importer;
					m_importer = nullptr;
				}
				return;
			}


			bool CAnimationClip::Init(const char* animFilePath, bool registerAnimBank)
			{
				m_isLoaded = false;

				ImportScene(animFilePath);

				m_isLoaded = true;

				if (registerAnimBank)
				{
					auto& animClipBank =
						nsMemory::CResourceBankTable::GetInstance()->GetAnimationClipBank();
					auto* animClip = animClipBank.Get(animFilePath);
					if (animClip == nullptr)
					{
						SetIsShared(true);
						// AnimBankに未登録のため、新規に登録する。
						animClipBank.Register(animFilePath, this);
					}
				}


				m_nodeAnimMapArray.resize(m_scene->mNumAnimations);

				for (unsigned int animIdx = 0; animIdx < m_scene->mNumAnimations; animIdx++)
				{
					auto* anim = m_scene->mAnimations[animIdx];
					auto& nodeAnimMap = m_nodeAnimMapArray[animIdx];
					for (unsigned int channelIdx = 0; channelIdx < anim->mNumChannels; channelIdx++)
					{
						const aiNodeAnim* pNodeAnim = anim->mChannels[channelIdx];
						nodeAnimMap[pNodeAnim->mNodeName.data] = 
							pNodeAnim;
					}
				}

				return true;
			}

			bool CAnimationClip::ImportScene(const char* animFilePath)
			{
				bool res = nsAssimpCommon::ImportScene(
					animFilePath,
					m_importer,
					m_scene,
					nsAssimpCommon::g_kAnimationRemoveFlags,
					nsAssimpCommon::g_kAnimationPostprocessFlags
				);

				return res;
			}


			void CAnimationClip::CalcAndGetAnimatedBoneTransforms(
				float timeInSeconds,
				std::vector<nsMath::CMatrix>* pMTransforms,
				CSkelton* pSkelton,
				unsigned int animIdx,
				bool isLoop
			) noexcept
			{
				if (pSkelton == nullptr)
				{
					return;
				}
				if (animIdx >= m_scene->mNumAnimations)
				{
					// 指定されたアニメーションインデックスが、アニメーションの数を超えてる。
#ifdef _DEBUG
					char buffer[256];
					sprintf_s(buffer, "Invalid animation index %d, max is %d\n", animIdx, m_scene->mNumAnimations);
					::OutputDebugStringA(buffer);
#endif
					return;
				}

				float animTimeTicks = CalcAnimationTimeTicks(timeInSeconds, animIdx, isLoop);
				const aiAnimation& animation = *m_scene->mAnimations[animIdx];

				/*ReadNodeHierarchy(
					animTimeTicks, *m_scene->mRootNode, nsMath::CMatrix::Identity(), animation, pSkelton);*/
				ReadNodeHierarchy(
					animTimeTicks,
					*(pSkelton->GetRootNode()->pNode),
					nsMath::CMatrix::Identity(), 
					animation,
					pSkelton,
					animIdx
				);

				const auto& boneInfoArray = pSkelton->GetBoneInfoArray();
				unsigned int numBoneInfoArray = static_cast<unsigned int>(boneInfoArray.size());
				
				if (pMTransforms->size() < numBoneInfoArray)
				{
					pMTransforms->resize(numBoneInfoArray);
				}


				for (unsigned int boneIdx = 0; boneIdx < numBoneInfoArray; boneIdx++)
				{
					(*pMTransforms)[boneIdx] = boneInfoArray[boneIdx].mFinalTransform;
				}

				return;
			}

			float CAnimationClip::CalcAnimationTimeTicks(
				float timeInSeconds, unsigned int animIdx, bool isLoop) noexcept
			{
				float ticksPerSecond =
					m_scene->mAnimations[animIdx]->mTicksPerSecond != 0 ?
					static_cast<float>(m_scene->mAnimations[animIdx]->mTicksPerSecond) : 25.0f;
				float timeInTicks = timeInSeconds * ticksPerSecond;

				// we need to use the integral part of mDuration for the total length of the animation
				float duration = 0.0f;
				float fraction = modf(
					static_cast<float>(m_scene->mAnimations[animIdx]->mDuration),
					&duration
				);

				float animTimeTicks = 0.0f;
				if (isLoop)
				{
					// ループ再生
					animTimeTicks = fmod(timeInTicks, duration);

					int animLoopCount = static_cast<int>(timeInTicks / duration);
					if (animLoopCount != m_animLoopCounter)
					{
						// 終端まで再生した。
						m_animLoopCounter = animLoopCount;
						m_prevAnimEventIdx = 0;
					}

					m_isPlayedAnimationToEnd = false;
				}
				else
				{
					// ワンショット再生
					animTimeTicks = std::min(timeInTicks, duration);

					if (timeInTicks > duration)
					{
						m_prevAnimEventIdx = 0;
						m_isPlayedAnimationToEnd = true;
					}
					else
					{
						m_isPlayedAnimationToEnd = false;
					}
				}

				return animTimeTicks;
			}

			void CAnimationClip::ReadNodeHierarchy(
				float animTimeTicks,
				const aiNode& node,
				const nsMath::CMatrix& parentTransform,
				const aiAnimation& animation,
				CSkelton* pSkelton,
				unsigned int animIdx
			) noexcept
			{
				nsMath::CMatrix mNodeTransform;
				nsAssimpCommon::AiMatrixToMyMatrix(node.mTransformation, &mNodeTransform);

				std::string nodeName(node.mName.data);
				const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, nodeName, animIdx);

				if (pNodeAnim)
				{
					nsAssimpCommon::SLocalTransform localTransform;
					CalcLocalTransform(localTransform, animTimeTicks, *pNodeAnim);

					nsMath::CMatrix mScale, mRot, mTrans;
					mScale.MakeScaling(
						localTransform.scaling.x,
						localTransform.scaling.y,
						localTransform.scaling.z);

					// aiMatrix3x3t から CMatrix の変換は用意してないから、
					// いったん aiMatrix4x4 に変換してから CMatrix に変換する。
					const auto& rotM3x3 = localTransform.rotation.GetMatrix();
					aiMatrix4x4 rotM4x4(rotM3x3);
					nsAssimpCommon::AiMatrixToMyMatrix(rotM4x4, &mRot);

					mTrans.MakeTranslation(
						localTransform.translation.x,
						localTransform.translation.y,
						localTransform.translation.z);

					// Combine the above transformations
					//NodeTransformation = TranslationM * RotationM * ScalingM;
					mNodeTransform = mScale * mRot * mTrans;
				}

				//nsMath::CMatrix GlobalTransformation = ParentTransform * NodeTransformation;
				nsMath::CMatrix mGlobalTransform = mNodeTransform * parentTransform;

				const auto& boneNameToIndexMap = pSkelton->GetBoneNameToIndexMap();
				auto itr = boneNameToIndexMap.find(nodeName);
				if (itr != boneNameToIndexMap.end())
				{
					unsigned int boneIdx = itr->second;
					/*m_boneInfo[BoneIndex].FinalTransformation =
						m_globalInverseTransform * GlobalTransformation * m_boneInfo[BoneIndex].OffsetMatrix;*/
					pSkelton->SetBoneFinalTransformMatrix(boneIdx, mGlobalTransform);
				}

				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++)
				{
					std::string childName(node.mChildren[childIdx]->mName.data);

					const auto& requiredNodeMap = pSkelton->GetRequiredNodeMap();
					const auto& it = requiredNodeMap.find(childName);

					if (it == requiredNodeMap.end())
					{
						if (childName == m_kAnimEventKeyNodeName)
						{
							ReadAnimKeyEventNode(
								animTimeTicks, *node.mChildren[childIdx], animation, animIdx);

							continue;
						}
#ifdef _DEBUG
						char buffer[256];
						sprintf_s(buffer, "Child %s cannot be found in the required node map\n", childName.c_str());
						::OutputDebugStringA(buffer);
#endif
						continue;
					}

					if (it->second.isRequired)
					{
						ReadNodeHierarchy(
							animTimeTicks,
							*node.mChildren[childIdx],
							mGlobalTransform,
							animation,
							pSkelton,
							animIdx
						);
					}
				}

				return;
			}



			const aiNodeAnim* CAnimationClip::FindNodeAnim(
				const aiAnimation& Animation,
				const std::string& NodeName,
				unsigned int animIdx
			) const noexcept
			{
				// クソ遅いのでunordered_mapに変更

				auto itr = m_nodeAnimMapArray[animIdx].find(NodeName);

				if (itr != m_nodeAnimMapArray[animIdx].end())
				{
					return itr->second;
				}

				//for (unsigned channelIdx = 0; channelIdx < Animation.mNumChannels; channelIdx++)
				//{
				//	const aiNodeAnim* pNodeAnim = Animation.mChannels[channelIdx];

				//	if (std::string(pNodeAnim->mNodeName.data) == NodeName)
				//	{
				//		return pNodeAnim;
				//	}
				//}

				return nullptr;
			}



			void CAnimationClip::CalcLocalTransform(
				nsAssimpCommon::SLocalTransform& localTransform,
				float animTimeTicks,
				const aiNodeAnim& nodeAnim
			) const noexcept
			{
				CalcInterpolatedScaling(&localTransform.scaling, animTimeTicks, nodeAnim);
				CalcInterpolatedRotation(&localTransform.rotation, animTimeTicks, nodeAnim);
				CalcInterpolatedPosition(&localTransform.translation, animTimeTicks, nodeAnim);
				return;
			}

			void CAnimationClip::CalcInterpolatedScaling(
				aiVector3D* pScaling, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumScalingKeys == 1)
				{
					*pScaling = nodeAnim.mScalingKeys[0].mValue;
					return;
				}

				unsigned int scalingIdx = FindScaling(animTimeTicks, nodeAnim);
				unsigned int nextScalingIdx = scalingIdx + 1;
				assert(nextScalingIdx < nodeAnim.mNumScalingKeys);

				float t1 = static_cast<float>(nodeAnim.mScalingKeys[scalingIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pScaling = nodeAnim.mScalingKeys[scalingIdx].mValue;
				}
				else
				{
					float t2 = static_cast<float>(nodeAnim.mScalingKeys[nextScalingIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiVector3D& start = nodeAnim.mScalingKeys[scalingIdx].mValue;
					const aiVector3D& end = nodeAnim.mScalingKeys[nextScalingIdx].mValue;
					aiVector3D delta = end - start;
					*pScaling = start + factor * delta;
				}

				return;
			}

			void CAnimationClip::CalcInterpolatedRotation(
				aiQuaternion* pRotation, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumRotationKeys == 1)
				{
					*pRotation = nodeAnim.mRotationKeys[0].mValue;
					return;
				}

				unsigned int rotationIdx = FindRotation(animTimeTicks, nodeAnim);
				unsigned int nextRotationIdx = rotationIdx + 1;
				assert(nextRotationIdx < nodeAnim.mNumRotationKeys);

				float t1 = static_cast<float>(nodeAnim.mRotationKeys[rotationIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pRotation = nodeAnim.mRotationKeys[rotationIdx].mValue;
				}
				else
				{
					float t2 = static_cast<float>(nodeAnim.mRotationKeys[nextRotationIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiQuaternion& startRotationQ = nodeAnim.mRotationKeys[rotationIdx].mValue;
					const aiQuaternion& endRotationQ = nodeAnim.mRotationKeys[nextRotationIdx].mValue;
					aiQuaternion::Interpolate(*pRotation, startRotationQ, endRotationQ, factor);
				}

				pRotation->Normalize();

				return;
			}

			void CAnimationClip::CalcInterpolatedPosition(
				aiVector3D* pPosition, float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				// we need at least two values to interpolate...
				if (nodeAnim.mNumPositionKeys == 1)
				{
					*pPosition = nodeAnim.mPositionKeys[0].mValue;
					return;
				}

				unsigned int positionIdx = FindPosition(animTimeTicks, nodeAnim);
				unsigned int nextPositionIdx = positionIdx + 1;
				assert(nextPositionIdx < nodeAnim.mNumPositionKeys);

				float t1 = static_cast<float>(nodeAnim.mPositionKeys[positionIdx].mTime);
				if (t1 > animTimeTicks)
				{
					*pPosition = nodeAnim.mPositionKeys[positionIdx].mValue;
				}
				else
				{
					float t2 = static_cast<float>(nodeAnim.mPositionKeys[nextPositionIdx].mTime);
					float deltaTime = t2 - t1;
					float factor = (animTimeTicks - t1) / deltaTime;
					assert(factor >= 0.0f && factor <= 1.0f);
					const aiVector3D& start = nodeAnim.mPositionKeys[positionIdx].mValue;
					const aiVector3D& end = nodeAnim.mPositionKeys[nextPositionIdx].mValue;
					aiVector3D delta = end - start;
					*pPosition = start + factor * delta;
				}

				return;
			}

			unsigned int CAnimationClip::FindScaling(
				float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				assert(nodeAnim.mNumScalingKeys > 0);

				unsigned int finalKeyIdx = 0;

				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumScalingKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mScalingKeys[keyIdx + 1].mTime);
					finalKeyIdx = keyIdx;
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return finalKeyIdx;
			}

			unsigned int CAnimationClip::FindRotation(
				float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				assert(nodeAnim.mNumRotationKeys > 0);

				unsigned int finalKeyIdx = 0;

				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumRotationKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mRotationKeys[keyIdx + 1].mTime);
					finalKeyIdx = keyIdx;
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return finalKeyIdx;
			}


			unsigned int CAnimationClip::FindPosition(
				float animTimeTicks, const aiNodeAnim& nodeAnim) const noexcept
			{
				unsigned int finalKeyIdx = 0;
				for (unsigned int keyIdx = 0; keyIdx < nodeAnim.mNumPositionKeys - 1; keyIdx++)
				{
					float t = static_cast<float>(nodeAnim.mPositionKeys[keyIdx + 1].mTime);
					finalKeyIdx = keyIdx;
					if (animTimeTicks < t)
					{
						return keyIdx;
					}
				}

				return finalKeyIdx;
			}





			void CAnimationClip::ReadAnimKeyEventNode(
				float animTimeTicks,
				const aiNode& node,
				const aiAnimation& animation,
				unsigned int animIdx
			) noexcept
			{
				if (IsPlayedAnimationToEnd())
				{
					// 最後まで再生し終わったら、イベントは実行されない。
					return;
				}

				std::string nodeName(node.mName.data);
				const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, nodeName, animIdx);

				if (pNodeAnim == nullptr)
				{
					// アニメーションイベントのアニメーションチャンネルがない。
					return;
				}

				// we need at least two values to interpolate...
				if (pNodeAnim->mNumPositionKeys == 1)
				{
					return;
				}

				// 現在のアニメーションのタイムから最も近い、かつ、
				// まだ過ぎていないキーインデックスを探す。
				unsigned int animEventKeyIdx = FindPosition(animTimeTicks, *pNodeAnim);

				if (animEventKeyIdx <= m_prevAnimEventIdx * 2)
				{
					// すでに実行したことがあるアニメーションイベント。
					return;
				}

				// アニメーションイベント実行。
				if (m_prevAnimEventIdx < m_animationEventFuncArray.size())
				{
					m_animationEventFuncArray[m_prevAnimEventIdx]();
				}

				// アニメーションイベントを呼ぶタイミングは、キーとキーの間に挟まれているため、
				// イベントを1つ実行したら、2つ進める。
				m_prevAnimEventIdx++;

				return;
			}


		}
	}
}