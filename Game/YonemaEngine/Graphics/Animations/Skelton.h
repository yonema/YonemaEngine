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

					nsMath::CMatrix mOffset;
					nsMath::CMatrix mGlobalTransform;
					nsMath::CMatrix mFinalTransform;
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
					unsigned int numVerteices
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
					nsMath::CVector3 scale;
					scale.x = mGlobalTransform.m_vec4Mat[0].Length();
					scale.y = mGlobalTransform.m_vec4Mat[1].Length();
					scale.z = mGlobalTransform.m_vec4Mat[2].Length();
					m_boneInfoArray[boneIdx].mFinalTransform = 
						m_boneInfoArray[boneIdx].mOffset * 
						mGlobalTransform * 
						m_mGlobalTransformInv;
				}

			private:

				void InitializeRequiredNodeMap(const aiNode& node);

				void CopyBaseVertexNoArray(unsigned int numMeshes, const std::vector<unsigned int>& baseVertexNoArray);

				void LoadMeshBones(const aiMesh& srcMesh, unsigned int meshIdx);

				void LoadSingleBone(const aiBone& bone, unsigned int meshIndex);

				unsigned int GetBoneId(const aiBone& bone);

				void MarkRequiredNodesForBone(const aiBone& bone);


			private:
				nsMath::CMatrix m_mGlobalTransformInv;
				std::unordered_map<std::string, SNodeInfo> m_requiredNodeMap = {};
				std::vector<SBoneInfo> m_boneInfoArray = {};
				std::unordered_map<std::string, unsigned int> m_boneNameToIndexMap = {};
				std::vector<SBasicMeshInfo>* m_meshesInfo = nullptr;
				std::vector<unsigned int> m_baseVertexNoArray = {};
				std::vector<SVertexBoneData> m_vertexBoneDataArray = {};
			};

		}
	}
}