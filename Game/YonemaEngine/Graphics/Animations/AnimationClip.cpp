#include "AnimationClip.h"
#include "../AssimpCommonHeader.h"
#include "Skelton.h"
#include "../../Utils/StringManipulation.h"
#include "../../Utils/AlignSize.h"

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
				if (m_importer)
				{
					m_importer->FreeScene();
					delete m_importer;
					m_importer = nullptr;
				}
				return;
			}


			bool CAnimationClip::Init(const char* animFilePath, CSkelton* pSkelton)
			{
				ImportScene(animFilePath);

				m_skeltonRef = pSkelton;

				return true;
			}

			bool CAnimationClip::ImportScene(const char* animFilePath)
			{
				// utf8のファイルパス文字列が必要なため変換。

				auto filePathInChar = animFilePath;
				auto filePathInWStr = nsUtils::GetWideStringFromString(filePathInChar);
				auto filePathInUTF8Str = nsUtils::ToUTF8(filePathInWStr);

				m_importer = new Assimp::Importer;
				unsigned int removeFlags =
					aiComponent_COLORS |
					aiComponent_TEXCOORDS |
					aiComponent_BONEWEIGHTS |
					aiComponent_TEXTURES |
					aiComponent_LIGHTS |
					aiComponent_CAMERAS |
					aiComponent_MESHES |
					aiComponent_MATERIALS;
				m_importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, removeFlags);

				m_importer->SetPropertyInteger(
					AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

				// インポートのポストプロセス設定。
				static constexpr int kPostprocessFlag =
					aiProcess_RemoveComponent | 
					aiProcess_MakeLeftHanded;


				m_scene = m_importer->ReadFile(filePathInUTF8Str, kPostprocessFlag);

				if (m_scene == nullptr)
				{
					std::wstring wstr = filePathInWStr;
					wstr += L"のモデルの読み込みに失敗しました。";
					nsGameWindow::MessageBoxWarning(wstr.c_str());
					::OutputDebugStringA(m_importer->GetErrorString());
					::OutputDebugStringA("\n");
					return false;
				}

				return true;
			}


			void CAnimationClip::CalcAndGetAnimatedBoneTransforms(
				float timeInSeconds,
				std::vector<nsMath::CMatrix>* pMTransforms,
				unsigned int animIdx,
				bool isLoop
			) noexcept
			{
				if (m_skeltonRef == nullptr)
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

				ReadNodeHierarchy(
					animTimeTicks, *m_scene->mRootNode, nsMath::CMatrix::Identity(), animation);

				const auto& boneInfoArray = m_skeltonRef->GetBoneInfoArray();
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
				const aiAnimation& animation
			) noexcept
			{
				nsMath::CMatrix mNodeTransform;
				nsAssimpCommon::AiMatrixToMyMatrix(node.mTransformation, &mNodeTransform);

				std::string nodeName(node.mName.data);
				const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, nodeName);

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

				const auto& boneNameToIndexMap = m_skeltonRef->GetBoneNameToIndexMap();
				if (boneNameToIndexMap.count(nodeName) > 0)
				{
					unsigned int boneIdx = boneNameToIndexMap.at(nodeName);
					/*m_boneInfo[BoneIndex].FinalTransformation =
						m_globalInverseTransform * GlobalTransformation * m_boneInfo[BoneIndex].OffsetMatrix;*/
					m_skeltonRef->SetBoneFinalTransformMatrix(boneIdx, mGlobalTransform);
				}

				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++)
				{
					std::string childName(node.mChildren[childIdx]->mName.data);

					const auto& requiredNodeMap = m_skeltonRef->GetRequiredNodeMap();
					const auto& it = requiredNodeMap.find(childName);

					if (it == requiredNodeMap.end())
					{
						if (childName == m_kAnimEventKeyNodeName)
						{
							ReadAnimKeyEventNode(animTimeTicks, *node.mChildren[childIdx], animation);

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
						ReadNodeHierarchy(animTimeTicks, *node.mChildren[childIdx], mGlobalTransform, animation);
					}
				}

				return;
			}



			const aiNodeAnim* CAnimationClip::FindNodeAnim(
				const aiAnimation& Animation, const std::string& NodeName) const noexcept
			{
				for (unsigned channelIdx = 0; channelIdx < Animation.mNumChannels; channelIdx++)
				{
					const aiNodeAnim* pNodeAnim = Animation.mChannels[channelIdx];

					if (std::string(pNodeAnim->mNodeName.data) == NodeName)
					{
						return pNodeAnim;
					}
				}

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
				const aiAnimation& animation
			) noexcept
			{
				if (IsPlayedAnimationToEnd())
				{
					// 最後まで再生し終わったら、イベントは実行されない。
					return;
				}

				std::string nodeName(node.mName.data);
				const aiNodeAnim* pNodeAnim = FindNodeAnim(animation, nodeName);

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