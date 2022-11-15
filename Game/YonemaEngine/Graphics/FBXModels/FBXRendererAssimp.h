#pragma once
#include "../Renderers/ModelRendererBase.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAssimpCommon
		{
			struct SLocalTransform;
		}
		namespace nsAnimations
		{
			class CSkelton;
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

				struct SBasicMeshInfo
				{
					constexpr SBasicMeshInfo() = default;

					unsigned int numIndices = 0;
					unsigned int baseVertexNo = 0;
					unsigned int baseIndexNo = 0;
					unsigned int materialIndex = 0;
				};









			public:
				CFBXRendererAssimp(const SModelInitData& modelInitData);
				~CFBXRendererAssimp();

				void Release();

			private:
				bool Init(const SModelInitData& modelInitData);

				void Terminate();

				bool ImportScene(const char* modelFilePath);

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

				void GetBoneTransforms(
					float timeInSeconds,
					std::vector<nsMath::CMatrix>* transforms,
					unsigned int animIdx = 0
				);

				float CalcAnimationTimeTicks(float timeInSeconds, unsigned int animIdx);

				void ReadNodeHierarchy(
					float animTimeTicks,
					const aiNode& node,
					const nsMath::CMatrix& parentTransform, 
					const aiAnimation& animation
				);

				const aiNodeAnim* FindNodeAnim(const aiAnimation&
					Animation, const std::string& NodeName);

				void CalcLocalTransform(
					nsAssimpCommon::SLocalTransform& localTransform,
					float animTimeTicks,
					const aiNodeAnim& nodeAnim
				);
				void CalcInterpolatedScaling(
					aiVector3D* pScaling, float animTimeTicks, const aiNodeAnim& nodeAnim);

				void CalcInterpolatedRotation(
					aiQuaternion* pRotation, float animTimeTicks, const aiNodeAnim& nodeAnim);

				void CalcInterpolatedPosition(
					aiVector3D* pPosition, float animTimeTicks, const aiNodeAnim& nodeAnim);

				unsigned int FindScaling(float animTimeTicks, const aiNodeAnim& nodeAnim);

				unsigned int FindRotation(float animTimeTicks, const aiNodeAnim& nodeAnim);

				unsigned int FindPosition(float animTimeTicks, const aiNodeAnim& nodeAnim);


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

				std::unordered_map<unsigned int, std::unordered_map<std::string, float>> m_boneNameAndWeightListTable;
				std::vector<SBasicMeshInfo> m_meshInfoArray;
				Assimp::Importer* m_importer = nullptr;
				const aiScene* m_scene = nullptr;;
				std::vector<nsMath::CMatrix> m_boneMatrices;
				float m_animationTimer = 0.0f;
				nsAnimations::CSkelton* m_skelton = nullptr;
			};

		}
	}
}