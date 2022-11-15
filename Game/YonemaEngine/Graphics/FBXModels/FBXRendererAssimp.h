#pragma once
#include "../Renderers/ModelRendererBase.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			struct SLocalTransform;
		}
		namespace nsRenderers
		{
			struct SModelInitData;
		}
	}
}
namespace Assimp
{
	class Importer;
}
typedef float ai_real;
template <typename TReal>
class aiVector3t;
typedef aiVector3t<ai_real> aiVector3D;
template <typename TReal>
class aiQuaterniont;
typedef aiQuaterniont<ai_real> aiQuaternion;

struct aiAnimation;
struct aiMesh;
struct aiMaterial;
struct aiNode;
struct aiBone;
struct aiScene;
struct aiNodeAnim;



namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsFBXModels
		{
			class CFBXRendererAssimp : public nsRenderers::IModelRendererBase
			{
			protected:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			public:
				void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) override final;

				void UpdateAnimation(float deltaTime) override final;

			private:
				struct SVertex
				{
					nsMath::CVector3 position;
					nsMath::CVector3 normal;
					nsMath::CVector4 color;
					nsMath::CVector2 uv;
					unsigned short boneNo[4];
					unsigned short weights[4];
				};

				struct SMesh
				{
					std::vector<SVertex> vertices;
					std::vector<uint16_t> indices;
					std::string diffuseMapFilePath;
				};

				struct SBasicMeshEntry 
				{
					SBasicMeshEntry()
					{
						NumIndices = 0;
						BaseVertex = 0;
						BaseIndex = 0;
						MaterialIndex = 0;
					}

					unsigned int NumIndices;
					unsigned int BaseVertex;
					unsigned int BaseIndex;
					unsigned int MaterialIndex;
				};

				struct SNodeInfo
				{

					SNodeInfo() {}

					SNodeInfo(const aiNode* n) { pNode = n; }

					const aiNode* pNode = NULL;
					bool isRequired = false;
				};

				struct SBoneInfo
				{
					nsMath::CMatrix OffsetMatrix;
					nsMath::CMatrix FinalTransformation;

					SBoneInfo(const nsMath::CMatrix& Offset)
					{
						OffsetMatrix = Offset;
						FinalTransformation = nsMath::CMatrix::Zero();
					}
				};

				struct SVertexBoneData
				{
					unsigned int BoneIDs[4] = { 0 };
					float Weights[4] = { 0.0f };
					int index = 0;  // slot for the next update

					SVertexBoneData()
					{
					}

					void AddBoneData(unsigned int BoneID, float Weight)
					{
						for (int i = 0; i < index; i++) {
							if (BoneIDs[i] == BoneID) {
								//  printf("bone %d already found at index %d old weight %f new weight %f\n", BoneID, i, Weights[i], Weight);
								return;
							}
						}

						// The iClone 7 Raptoid Mascot (https://sketchfab.com/3d-models/iclone-7-raptoid-mascot-free-download-56a3e10a73924843949ae7a9800c97c7)
						// has a problem of zero weights causing an overflow and the assertion below. This fixes it.
						if (Weight == 0.0f) {
							return;
						}

						// printf("Adding bone %d weight %f at index %i\n", BoneID, Weight, index);

						if (index == 4) {
							return;
							assert(0);
						}

						BoneIDs[index] = BoneID;
						Weights[index] = Weight;

						index++;
					}
				};




			public:
				CFBXRendererAssimp(const SModelInitData& modelInitData);
				~CFBXRendererAssimp();

				void Release();

			private:
				bool Init(const SModelInitData& modelInitData);

				void Terminate();

				void LoadMeshBones(const aiMesh& srcMesh, unsigned int meshIdx);

				void LoadSingleBone(const aiBone* pBone, unsigned int MeshIndex);

				int GetBoneId(const aiBone* pBone);

				void LoadMesh(SMesh* dstMesh, const aiMesh& srcMesh, unsigned int meshIdx);

				void LoadTexture(
					SMesh* dxtMesh, 
					const aiMaterial& srcMaterial,
					const char* modelFilePath, 
					unsigned int meshIdx
				);

				bool CreateVertexAndIndexBuffer(const std::vector<SMesh>& meshes);

				bool CreateModelCBV();

				bool CreateMaterialSRV();

				void InitializeRequiredNodeMap(const aiNode* pNode);

				void MarkRequiredNodesForBone(const aiBone* pBone);

				void GetBoneTransforms(
					float TimeInSeconds,
					std::vector<nsMath::CMatrix>* Transforms,
					unsigned int AnimationIndex = 0
				);

				float CalcAnimationTimeTicks(float TimeInSeconds, unsigned int AnimationIndex);

				void ReadNodeHierarchy(
					float AnimationTimeTicks,
					const aiNode* pNode,
					const nsMath::CMatrix& ParentTransform, 
					const aiAnimation& Animation
				);

				const aiNodeAnim* FindNodeAnim(const aiAnimation&
					Animation, const std::string& NodeName);

				void CalcLocalTransform(
					SLocalTransform& Transform,
					float AnimationTimeTicks,
					const aiNodeAnim* pNodeAnim
				);
				void CalcInterpolatedScaling(
					aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

				void CalcInterpolatedRotation(
					aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

				void CalcInterpolatedPosition(
					aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

				unsigned int FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

				unsigned int FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);

				unsigned int FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);


			private:
				std::vector<nsDx12Wrappers::CVertexBuffer*> m_vertexBuffers;
				std::vector<nsDx12Wrappers::CIndexBuffer*> m_indexBuffers;
				std::vector<unsigned int> m_numIndicesArray;

				nsDx12Wrappers::CConstantBuffer m_modelCB;
				nsDx12Wrappers::CDescriptorHeap m_modelDH;

				std::vector<std::string> m_materialNameTable;
				std::unordered_map <std::string, nsDx12Wrappers::CTexture*> m_diffuseTextures;
				std::unordered_map <std::string, nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;

				nsMath::CMatrix m_bias;
				nsMath::CMatrix m_globalInverseTransform;

				std::unordered_map<unsigned int, std::unordered_map<std::string, float>> m_boneNameAndWeightListTable;
				std::vector<SBoneInfo> m_boneInfo;
				std::vector<SVertexBoneData> m_bones;
				std::vector<SBasicMeshEntry> m_meshes;
				std::unordered_map<std::string, SNodeInfo> m_requiredNodeMap;
				std::unordered_map<std::string, unsigned int> m_BoneNameToIndexMap;
				Assimp::Importer* m_importer = nullptr;
				const aiScene* m_scene = nullptr;;
				std::vector<nsMath::CMatrix> m_boneMatrices;
				float m_animationTimer = 0.0f;
			};

		}
	}
}