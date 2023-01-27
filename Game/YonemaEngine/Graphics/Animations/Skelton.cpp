#include "Skelton.h"
#include "../Models/AssimpCommon.h"


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			std::unordered_map<std::string, std::unordered_map<std::string, float>>
				CSkelton::m_retargetScaleBase = {};


			void CSkelton::SVertexBoneData::AddBoneData(unsigned int boneID, float weight) noexcept
			{
				if (CheckForDuplicateBones(boneID, weight) != true ||
					CheckForWeightZeroBone(weight) != true ||
					CheckForExceedingBoneLimitWeight() != true)
				{
					return;
				}
#ifdef _DEBUG
				// char buffer[256];
				// sprintf_s(buffer, "Adding bone %d weight %f at index %i\n", boneID, weight, index);
				// ::OutputDebugStringA(buffer);
#endif

				boneIDs[index] = static_cast<unsigned short>(boneID);
				weights[index] = static_cast<unsigned short>(weight* 10000.0f);

				index++;

				return;
			}

			bool CSkelton::SVertexBoneData::CheckForDuplicateBones(
				unsigned int boneID, float weight) const noexcept
			{
				// 重複しているボーンを設定しようとしていないかチェック。
				for (unsigned short i = 0; i < index; i++)
				{
					if (boneIDs[i] == boneID)
					{
#ifdef _DEBUG
						char buffer[256];
						sprintf_s(
							buffer,
							"bone %d already found at index %d old weight %i new weight %f\n",
							boneID, i, weights[i], weight);
						::OutputDebugStringA(buffer);
#endif
						return false;
					}
				}

				return true;
			}

			bool CSkelton::SVertexBoneData::CheckForWeightZeroBone(float weight) const noexcept
			{
				// ウェイトが0なボーンを設定しようとしていないかチェック。
				// has a problem of zero weights causing an overflow and the assertion below. This fixes it.
				if (weight == 0.0f)
				{
#ifdef _DEBUG
					::OutputDebugStringA("Tried to set a weight 0 bone.\n");
#endif
					return false;
				}

				return true;
			}

			bool CSkelton::SVertexBoneData::CheckForExceedingBoneLimitWeight() const noexcept
			{
				// ボーンの制限数をオーバーしていないかチェック。
				if (index == m_kBoneLimitWeight)
				{
					::OutputDebugStringA("Bone limit has been exceeded.\n");
					return false;
				}

				return true;
			}




			void CSkelton::Init(const aiNode& rootNode)
			{
				const auto& mGlobalTransform = rootNode.mTransformation;
				nsAssimpCommon::AiMatrixToMyMatrix(mGlobalTransform, &m_mGlobalTransformInv);
				m_mGlobalTransformInv.Inverse();


				InitializeRequiredNodeMap(rootNode);

				std::string nodeName(rootNode.mName.C_Str());

				m_rootNodeInfo = &m_requiredNodeMap[nodeName];

				return;
			}

			void CSkelton::InitializeRequiredNodeMap(const aiNode& node)
			{
				std::string nodeName(node.mName.C_Str());

				SNodeInfo nodeInfo(node);

				m_requiredNodeMap[nodeName] = nodeInfo;

				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++)
				{
					InitializeRequiredNodeMap(*node.mChildren[childIdx]);
				}
				return;
			}


			void CSkelton::LoadBones(
				unsigned int numMeshes,
				const aiMesh* const* const& meshes,
				const std::vector<unsigned int>& baseVertexNoArray,
				unsigned int numVerteices,
				const std::string& retargetSkeltonName
			)
			{
				// 全てのメッシュのボーンをロードする。

				CopyBaseVertexNoArray(numMeshes, baseVertexNoArray);

				m_vertexBoneDataArray.resize(numVerteices);

				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					// メッシュを1つずつ取り出して、ボーンをロードする。
					const auto& mesh = meshes[meshIdx];
					LoadMeshBones(*mesh, meshIdx);
				}

				if (retargetSkeltonName.empty())
				{
					return;
				}

				m_retargetSkeltonName = &retargetSkeltonName;

				std::unordered_map<std::string, float>* pRetargetBase = nullptr;
				auto retargetItr = m_retargetScaleBase.find(retargetSkeltonName);
				if (retargetItr == m_retargetScaleBase.end())
				{
					m_retargetScaleBase.emplace(retargetSkeltonName, std::unordered_map<std::string, float>());
					auto itr = m_retargetScaleBase.find(retargetSkeltonName);
					pRetargetBase = &itr->second;
				}

				const aiNode* pNode = m_rootNodeInfo->pNode;
				pNode = PreInitSkelton(*pNode);
				auto boneNameToIndexitr = m_boneNameToIndexMap.find(pNode->mName.data);

				if (boneNameToIndexitr != m_boneNameToIndexMap.end())
				{
					auto boneIdx = boneNameToIndexitr->second;
					const auto& nodeTransform = m_boneInfoArray[boneIdx].mOffset;
					InitSkeltonLength(*pNode, nsMath::CMatrix::Identity(), pRetargetBase);
				}




				return;
			}

			void CSkelton::CopyBaseVertexNoArray(
				unsigned int numMeshes, const std::vector<unsigned int>& baseVertexNoArray)
			{
				m_baseVertexNoArray.reserve(numMeshes);

				for (unsigned int meshIdx = 0; meshIdx < numMeshes; meshIdx++)
				{
					m_baseVertexNoArray.emplace_back(baseVertexNoArray[meshIdx]);
				}

				return;
			}



			void CSkelton::LoadMeshBones(const aiMesh& srcMesh, unsigned int meshIdx)
			{
				// 1つのメッシュのボーンをロードする。

				for (unsigned int boneIdx = 0; boneIdx < srcMesh.mNumBones; boneIdx++)
				{
					// ボーンを1つずつロードする。
					LoadSingleBone(*srcMesh.mBones[boneIdx], meshIdx);
				}

				return;
			}

			void CSkelton::LoadSingleBone(const aiBone& bone, unsigned int meshIndex)
			{
				// 1つのボーンをロードする。

				unsigned int boneId = GetBoneId(bone);

				if (boneId == m_boneInfoArray.size())
				{
					nsMath::CMatrix offsetMatrix;
					nsAssimpCommon::AiMatrixToMyMatrix(bone.mOffsetMatrix, &offsetMatrix);
					SBoneInfo boneInfo(offsetMatrix);
					m_boneInfoArray.emplace_back(boneInfo);
				}

				for (unsigned int i = 0; i < bone.mNumWeights; i++)
				{
					const aiVertexWeight& vw = bone.mWeights[i];
					unsigned int GlobalVertexID = m_baseVertexNoArray[meshIndex] + bone.mWeights[i].mVertexId;
					m_vertexBoneDataArray[GlobalVertexID].AddBoneData(boneId, vw.mWeight);
				}

				MarkRequiredNodesForBone(bone);
			}

			unsigned int CSkelton::GetBoneId(const aiBone& bone)
			{
				unsigned int boneIndex = 0;
				std::string boneName(bone.mName.C_Str());

				if (m_boneNameToIndexMap.count(boneName) == 0)
				{
					// Allocate an index for a new bone
					boneIndex = static_cast<unsigned int>(m_boneNameToIndexMap.size());
					m_boneNameToIndexMap.emplace(boneName, boneIndex);
				}
				else
				{
					boneIndex = m_boneNameToIndexMap[boneName];
				}

				return boneIndex;
			}

			void CSkelton::MarkRequiredNodesForBone(const aiBone& bone)
			{
				std::string nodeName(bone.mName.C_Str());
				const aiNode* pParent = nullptr;

				do 
				{
					std::unordered_map<std::string, SNodeInfo>::iterator requiredNodeItr =
						m_requiredNodeMap.find(nodeName);

					if (requiredNodeItr == m_requiredNodeMap.end())
					{
						// メッシュに設定してあるボーンが、ノードヒエラルキーの中にない。
#ifdef _DEBUG
						char buffer[256];
						sprintf_s(buffer, "Cannot find bone %s in the hierarchy\n", nodeName.c_str());
						::OutputDebugStringA(buffer);
#endif
						return;
					}

					if (requiredNodeItr->second.isRequired)
					{
						break;
					}

					requiredNodeItr->second.isRequired = true;
					pParent = requiredNodeItr->second.pNode->mParent;

					if (pParent)
					{
						auto tmpName = std::string(pParent->mName.C_Str());
						if (nodeName == tmpName)
						{
							// 親に自分自身を設定してあった場合、無限ループになるのでbreakする。
							break;
						}
						nodeName = tmpName;
					}

				} while (pParent);

				return;
			}

			const aiNode* CSkelton::PreInitSkelton(const aiNode& node) const noexcept
			{
				std::string nodeName(node.mName.data);
				auto boneNameToIndexitr = m_boneNameToIndexMap.find(nodeName);
				if (boneNameToIndexitr != m_boneNameToIndexMap.end())
				{
					return &node;
				}

				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++)
				{
					auto* resPNode = PreInitSkelton(*node.mChildren[childIdx]);
					if (resPNode != nullptr)
					{
						return resPNode;
					}
				}

				return nullptr;
			}

			void CSkelton::InitSkeltonLength(
				const aiNode& node,
				const nsMath::CMatrix& parentTransform,
				std::unordered_map<std::string, float>* pRetargetBase
			)
			{
				std::string nodeName(node.mName.data);

				auto boneNameToIndexitr = m_boneNameToIndexMap.find(nodeName);

				nsMath::CMatrix nodeTransform = parentTransform;

				if (boneNameToIndexitr != m_boneNameToIndexMap.end())
				{
					unsigned int boneIdx = boneNameToIndexitr->second;
					nodeTransform = m_boneInfoArray[boneIdx].mOffset;

					nsMath::CVector3 pos(
						nodeTransform.m_vec4Mat[3].x,
						nodeTransform.m_vec4Mat[3].y,
						nodeTransform.m_vec4Mat[3].z
					);

					nsMath::CVector3 parentPos(
						parentTransform.m_vec4Mat[3].x,
						parentTransform.m_vec4Mat[3].y,
						parentTransform.m_vec4Mat[3].z
					);

					auto diff = pos - parentPos;
					m_boneInfoArray[boneIdx].length = diff.Length();
					if (pRetargetBase)
					{
						(*pRetargetBase)[nodeName] = m_boneInfoArray[boneIdx].length;
					}
				}


				for (unsigned int childIdx = 0; childIdx < node.mNumChildren; childIdx++)
				{
					std::string childName(node.mChildren[childIdx]->mName.data);

					const auto& requiredNodeItr = m_requiredNodeMap.find(childName);
					if (requiredNodeItr == m_requiredNodeMap.end())
					{
						continue;
					}

					if (requiredNodeItr->second.isRequired)
					{
						InitSkeltonLength(*node.mChildren[childIdx], nodeTransform, pRetargetBase);
					}
				}

				return;
			}

			float CSkelton::GetAnimationScaled(
				const std::string& nodeName,
				unsigned int boneIdx) const noexcept
			{
				if (m_retargetSkeltonName == nullptr)
				{
					// リターゲット用のスケルトンを使用するように設定されていない
					return 1.0f;
				}

				auto retargetBaseItr = m_retargetScaleBase.find(*m_retargetSkeltonName);
				if (retargetBaseItr == m_retargetScaleBase.end())
				{
					// リターゲットのベースとなるスケルトンが設定されていない
					return 1.0f;
				}

				const auto& retargetBase = retargetBaseItr->second;
				auto itr = retargetBase.find(nodeName);

				if (itr == retargetBase.end())
				{
					// リターゲットのベースに対象のボーンがない
					return 1.0f;
				}

				if (fabsf(itr->second) <= FLT_EPSILON ||
					fabsf(m_boneInfoArray[boneIdx].length) <= FLT_EPSILON)
				{
					// 長さ0は困る
					return 1.0f;
				}

				return m_boneInfoArray[boneIdx].length / itr->second;
			}


		
		}
	}
}