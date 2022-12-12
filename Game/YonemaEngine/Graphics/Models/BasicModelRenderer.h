#pragma once
#include "../Renderers/Renderer.h"
#include "../Animations/Animator.h"
#include "../Animations/Skelton.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			struct SModelInitData;
		}
	}
}

// Assimp

struct aiScene;
struct aiMaterial;


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{

			class CBasicModelRenderer : public nsRenderers::IRenderer
			{
			private:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override;

			private:

				static const unsigned int m_kMaxNumBoneWeights = 4;
				static const unsigned int m_kAligSizeVertexStride = 4;
				static const unsigned int m_kMaxNumBones = 256;

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SVertex
				{
					constexpr SVertex() = default;
					~SVertex() = default;

					nsMath::CVector3 position;
					nsMath::CVector3 normal;
					nsMath::CVector4 color;
					nsMath::CVector2 uv;
					unsigned short boneNo[4] = {};
					unsigned short weights[4] = {};
				};

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SMesh
				{
					constexpr SMesh() = default;
					~SMesh() = default;

					std::vector<SVertex> vertices = {};
					std::vector<uint16_t> indices = {};
					std::string diffuseMapFilePath = {};
				};

				/**
				 * @attention この構造体はコピーを許可する。
				*/
				struct SBasicMeshInfo
				{
					constexpr SBasicMeshInfo() = default;
					constexpr SBasicMeshInfo(
						unsigned int numIndices,
						unsigned int baseVertexNo,
						unsigned int baseIndexNo,
						unsigned int materialIndex
					)
						: numIndices(numIndices),
						baseVertexNo(baseVertexNo),
						baseIndexNo(baseIndexNo),
						materialIndex(materialIndex)
					{};
					~SBasicMeshInfo() = default;

					unsigned int numIndices = 0;
					unsigned int baseVertexNo = 0;
					unsigned int baseIndexNo = 0;
					unsigned int materialIndex = 0;
				};



			public:
				CBasicModelRenderer(const nsRenderers::SModelInitData& modelInitData);
				~CBasicModelRenderer();

				void Release();

				void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) noexcept;

				void UpdateAnimation(float deltaTime) noexcept;

				inline void PlayAnimation(unsigned int animIdx) noexcept
				{
					if (m_animator)
					{
						m_animator->PlayAnimation(animIdx);
					}
				}

				inline bool IsPlaying() const noexcept
				{
					return m_animator ? m_animator->IsPlaying() : false;
				}

				inline void SetAnimationSpeed(float animSpeed) noexcept
				{
					if (m_animator)
					{
						m_animator->SetAnimationSpeed(animSpeed);
					}
				}

				inline void SetIsAnimationLoop(bool isLoop) noexcept
				{
					if (m_animator)
					{
						m_animator->SetIsLoop(isLoop);
					}
				}

				inline void ReserveAnimationEventFuncArray(
					unsigned int animIdx, unsigned int size) noexcept
				{
					if (m_animator)
					{
						m_animator->ReserveAnimationEventFuncArray(animIdx, size);
					}
				}

				inline void AddAnimationEventFunc(
					unsigned int animIdx,
					const std::function<void(void)>& animationEventFunc) noexcept
				{
					if (m_animator)
					{
						m_animator->AddAnimationEventFunc(animIdx, animationEventFunc);
					}
				}

				unsigned int FindBoneId(const std::string& boneName) const noexcept;

				inline const nsMath::CMatrix& GetBoneMatrix(
					unsigned int boneId) const noexcept
				{
					return m_skelton ?
						m_skelton->GetBoneInfoArray()[boneId].mGlobalTransform :
						nsMath::CMatrix::Identity();
				}

				inline const nsMath::CMatrix& GetWorldMatrix() const noexcept
				{
					return m_worldMatrix;
				}

				constexpr bool IsSkeltalAnimationValid() const noexcept
				{
					return m_animator && m_skelton;
				}

			private:
				bool Init(const nsRenderers::SModelInitData& modelInitData) noexcept;

				void Terminate() noexcept;

				bool InitSkeltalAnimation(
					const nsRenderers::SModelInitData& modelInitData, const aiScene* scene) noexcept;

				void InitMeshInfoArray(
					const aiScene* scene,
					const unsigned int numMeshes,
					unsigned int* numVerticesOut,
					unsigned int* numIndicesOut,
					std::vector<unsigned int>* baseVertexNoArrayOut = nullptr
				) noexcept;

				void LoadMeshes(
					const aiScene* scene,
					std::vector<SMesh>* destMeshesOut,
					const unsigned int numMeshes
				) noexcept;

				void LoadMesh(SMesh* dstMesh, const aiMesh& srcMesh, unsigned int meshIdx) noexcept;

				void LoadMaterials(
					const nsRenderers::SModelInitData& modelInitData, const aiScene* scene) noexcept;

				void LoadTexture(
					const nsRenderers::SModelInitData& modelInitData,
					const aiMaterial& srcMaterial,
					const char* aiMaterialKey,
					unsigned int aiMaterialType,
					unsigned int aiMaterialIndex,
					std::vector<nsDx12Wrappers::CTexture*>* texturesOut,
					unsigned int matIdx
				) noexcept;

				std::string BuildTextureFilePath(
					const nsRenderers::SModelInitData& modelInitData,
					const std::string texFileName
				) const noexcept;

				bool CreateVertexAndIndexBuffer(const std::vector<SMesh>& meshes) noexcept;

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

				nsDx12Wrappers::CConstantBuffer m_modelCB;
				nsDx12Wrappers::CDescriptorHeap m_modelDH;

				std::vector<nsDx12Wrappers::CTexture*> m_diffuseTextures;
				std::vector<nsDx12Wrappers::CDescriptorHeap*> m_materialDHs;

				nsMath::CMatrix m_bias = nsMath::CMatrix::Identity();
				nsMath::CMatrix m_worldMatrix = nsMath::CMatrix::Identity();

				std::vector<SBasicMeshInfo> m_meshInfoArray;
				std::vector<nsMath::CMatrix> m_boneMatrices;
				nsAnimations::CSkelton* m_skelton = nullptr;
				nsAnimations::CAnimator* m_animator = nullptr;
			};

		}
	}
}