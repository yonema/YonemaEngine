#pragma once
#include "../Renderers/ModelRendererBase.h"
#include "../Animations/Animator.h"
#include "../Animations/Skelton.h"

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


struct aiScene;
struct aiAnimation;
struct aiMesh;
struct aiMaterial;
struct aiNode;
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

				inline void PlayAnimation(unsigned int animIdx) noexcept override final
				{
					if (m_animator)
					{
						m_animator->PlayAnimation(animIdx);
					}
				}

				inline bool IsPlaying() const noexcept override final
				{
					return m_animator ? m_animator->IsPlaying() : false;
				}

				inline void SetAnimationSpeed(float animSpeed) noexcept override final
				{
					if (m_animator)
					{
						m_animator->SetAnimationSpeed(animSpeed);
					}
				}

				inline void SetIsAnimationLoop(bool isLoop) noexcept override final
				{
					if (m_animator)
					{
						m_animator->SetIsLoop(isLoop);
					}
				}

				inline void ReserveAnimationEventFuncArray(
					unsigned int animIdx, unsigned int size) override final
				{
					if (m_animator)
					{
						m_animator->ReserveAnimationEventFuncArray(animIdx, size);
					}
				}

				inline void AddAnimationEventFunc(
					unsigned int animIdx,
					const std::function<void(void)>& animationEventFunc) override final
				{
					if (m_animator)
					{
						m_animator->AddAnimationEventFunc(animIdx, animationEventFunc);
					}
				}

				unsigned int FindBoneId(const std::string& boneName) const noexcept override final;

				inline const nsMath::CMatrix& GetBoneMatrix(
					unsigned int boneId) const noexcept override final
				{
					return m_skelton ? 
						m_skelton->GetBoneInfoArray()[boneId].mGlobalTransform :
						nsMath::CMatrix::Identity();
				}

				inline const nsMath::CMatrix& GetWorldMatrix() const noexcept override final
				{
					return m_worldMatrix;
				}


			private:

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SVertex
				{
					nsMath::CVector3 position;
					nsMath::CVector3 normal;
					nsMath::CVector4 color;
					nsMath::CVector2 uv;
					unsigned short boneNo[4];
					unsigned short weights[4];
				};

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SMesh
				{
					std::vector<SVertex> vertices;
					std::vector<uint16_t> indices;
					std::string diffuseMapFilePath;
				};

				/**
				 * @attention この構造体はコピーを許可する。
				*/
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
				bool Init(const nsRenderers::SModelInitData& modelInitData);

				void Terminate();

				bool ImportScene(
					const char* modelFilePath, Assimp::Importer*& pImporter, const aiScene*& pScene);

				void LoadMesh(SMesh* dstMesh, const aiMesh& srcMesh, unsigned int meshIdx);

				void LoadTexture(
					SMesh* dxtMesh, 
					const aiMaterial& srcMaterial,
					const nsRenderers::SModelInitData& modelInitData,
					unsigned int meshIdx
				);

				bool CreateVertexAndIndexBuffer(const std::vector<SMesh>& meshes);

				void CopyToPhysicsMeshGeometryBuffer(
					const std::vector<SMesh>& meshes,
					const nsRenderers::SModelInitData& modelInitData,
					unsigned int numVertices,
					unsigned int numIndices
					);

				bool CreateModelCBV();

				bool CreateMaterialSRV();


			private:
				std::vector<nsDx12Wrappers::CVertexBuffer*> m_vertexBuffers;
				std::vector<nsDx12Wrappers::CIndexBuffer*> m_indexBuffers;
				std::vector<unsigned int> m_numIndicesArray;

				nsDx12Wrappers::CConstantBuffer m_modelCB;
				nsDx12Wrappers::CDescriptorHeap m_modelDH;

				std::vector<std::string> m_materialNameTable;
				//std::unordered_map <std::string, nsDx12Wrappers::CTexture*> m_diffuseTextures;
				std::vector <nsDx12Wrappers::CTexture*> m_diffuseTextures;
				//std::unordered_map <std::string, nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;
				std::vector <nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;

				nsMath::CMatrix m_bias = nsMath::CMatrix::Identity();
				nsMath::CMatrix m_worldMatrix = nsMath::CMatrix::Identity();

				std::unordered_map<unsigned int, std::unordered_map<std::string, float>> m_boneNameAndWeightListTable;
				std::vector<SBasicMeshInfo> m_meshInfoArray;
				std::vector<nsMath::CMatrix> m_boneMatrices;
				nsAnimations::CSkelton* m_skelton = nullptr;
				nsAnimations::CAnimator* m_animator = nullptr;
			};

		}
	}
}