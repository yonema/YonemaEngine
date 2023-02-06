#pragma once

struct aiNode;
struct aiBone;
struct SBasicMeshInfo;
struct aiMesh;

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CSkelton : nsUtils::SNoncopyable
			{
			public:
				static const unsigned int m_kNotFoundBoneID = UINT_MAX;

			private:

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SNodeInfo
				{
					constexpr SNodeInfo() = default;

					constexpr SNodeInfo(const aiNode& node) noexcept
						:pNode(&node)
					{};

					const aiNode* pNode = nullptr;
					bool isRequired = false;
				};

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SBoneInfo
				{
					constexpr SBoneInfo(const nsMath::CMatrix& offset) noexcept
						:mOffset(offset),
						mFinalTransform(nsMath::CMatrix::Identity())
					{};

					nsMath::CMatrix mOffset = nsMath::CMatrix::Identity();
					nsMath::CMatrix mGlobalTransform = nsMath::CMatrix::Identity();
					nsMath::CMatrix mFinalTransform = nsMath::CMatrix::Identity();
					float length = 0.0f;
				};

				struct SVertexBoneData
				{
					static const unsigned short m_kBoneLimitWeight = 4;

					constexpr SVertexBoneData() = default;

					void AddBoneData(unsigned int boneID, float weight) noexcept;

				private:
					bool CheckForDuplicateBones(unsigned int boneID, float weight) const noexcept;

					bool CheckForWeightZeroBone(float weight) const noexcept;

					bool CheckForExceedingBoneLimitWeight() const noexcept;

				public:
					unsigned short boneIDs[m_kBoneLimitWeight] = { 0 };
					unsigned short weights[m_kBoneLimitWeight] = { 0 };
					unsigned short index = 0;  // slot for the next update
				};


			public:
				constexpr CSkelton() = default;
				~CSkelton() = default;

				void Init(const aiNode& rootNode);

				void LoadBones(
					unsigned int numMeshes, 
					const aiMesh* const* const& meshes,
					const std::vector<unsigned int>& baseVertexNoArray,
					unsigned int numVerteices,
					const std::string& retargetSkeltonName
				);

				_CONSTEXPR20_CONTAINER unsigned short GetVertexBoneID(
					unsigned int globalVertexID, unsigned int boneIdx) const noexcept
				{
					return m_vertexBoneDataArray[globalVertexID].boneIDs[boneIdx];
				}
				_CONSTEXPR20_CONTAINER unsigned short GetVertexWeight(
					unsigned int globalVertexID, unsigned int boneIdx) const noexcept
				{
					return m_vertexBoneDataArray[globalVertexID].weights[boneIdx];
				}

				constexpr const auto& GetBoneInfoArray() const noexcept
				{
					return m_boneInfoArray;
				}

				constexpr const auto& GetBoneNameToIndexMap() const noexcept
				{
					return m_boneNameToIndexMap;
				}

				constexpr const auto& GetRequiredNodeMap() const noexcept
				{
					return m_requiredNodeMap;
				}

				inline void SetBoneFinalTransformMatrix(
					unsigned int boneIdx, const nsMath::CMatrix& mGlobalTransform) noexcept
				{
					m_boneInfoArray[boneIdx].mGlobalTransform = mGlobalTransform;
					m_boneInfoArray[boneIdx].mFinalTransform = 
						m_boneInfoArray[boneIdx].mOffset * 
						mGlobalTransform * 
						m_mGlobalTransformInv;
				}

				constexpr auto* GetRootNode() noexcept
				{
					return m_rootNodeInfo;
				}

				float GetAnimationScaled(
					const std::string& nodeName, unsigned int boneIdx) const noexcept;

				constexpr bool IsLoaded() const noexcept
				{
					return m_isLoaded;
				}

			private:

				void InitializeRequiredNodeMap(const aiNode& node);

				void CopyBaseVertexNoArray(unsigned int numMeshes, const std::vector<unsigned int>& baseVertexNoArray);

				void LoadMeshBones(const aiMesh& srcMesh, unsigned int meshIdx);

				void LoadSingleBone(const aiBone& bone, unsigned int meshIndex);

				unsigned int GetBoneId(const aiBone& bone);

				void MarkRequiredNodesForBone(const aiBone& bone);

				const aiNode* PreInitSkelton(const aiNode& node) const noexcept;

				void InitSkeltonLength(
					const aiNode& node, 
					const nsMath::CMatrix& parentTransform,
					std::unordered_map<std::string, float>* pRetargetBase = nullptr
				);


			private:
				static std::unordered_map<std::string, std::unordered_map<std::string, float>>
					m_retargetScaleBase;
				nsMath::CMatrix m_mGlobalTransformInv;
				std::unordered_map<std::string, SNodeInfo> m_requiredNodeMap = {};
				SNodeInfo* m_rootNodeInfo = nullptr;
				std::vector<SBoneInfo> m_boneInfoArray = {};
				std::unordered_map<std::string, unsigned int> m_boneNameToIndexMap = {};
				std::vector<SBasicMeshInfo>* m_meshesInfo = nullptr;
				std::vector<unsigned int> m_baseVertexNoArray = {};
				std::vector<SVertexBoneData> m_vertexBoneDataArray = {};
				const std::string* m_retargetSkeltonName = nullptr;
				bool m_isLoaded = false;
			};

		}
	}
}