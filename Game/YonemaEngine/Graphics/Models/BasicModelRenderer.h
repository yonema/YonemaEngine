#pragma once
#include "../Renderers/Renderer.h"
#include "../Animations/Animator.h"
#include "../Animations/Skelton.h"
#include "../Geometries/GeometryData.h"
#include "../Shadow/ShadowModelRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			struct SModelInitData;
		}
	}
	namespace nsGeometries
	{
		class CGeometryData;
	}
}

// Assimp
namespace Assimp
{
	class Importer;
}
struct aiScene;
struct aiMaterial;
struct aiNode;


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			/**
			 * @attention この構造体はコピーを許可する。
			*/
			struct SVertex
			{
				constexpr SVertex() = default;
				~SVertex() = default;

				nsMath::CVector3 position;
				nsMath::CVector3 normal;
				nsMath::CVector3 tangent;
				nsMath::CVector3 biNormal;
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
				nsMath::CMatrix mNodeTransformInv = nsMath::CMatrix::Identity();
			};

			class CBasicModelRenderer : public nsRenderers::IRenderer
			{
			private:
				void Draw(nsDx12Wrappers::CCommandList* commandList) override final;

			private:

				static const unsigned int m_kMaxNumBoneWeights = 4;
				static const unsigned int m_kAligSizeVertexStride = 4;
				static const unsigned int m_kMaxNumBones = 256;

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

				struct SConstantBufferData
				{
					nsMath::CMatrix mWorld;
					nsMath::CMatrix mViewProj;
					nsMath::CMatrix mLightViewProj;
					nsMath::CVector3 lightPos;
					bool isShadowReceiver;
				};

				enum class EnDescHeapLayoutPerModel
				{
					enModelCBV,
					enExpandCBV,
					enShadowMapSRV,
					enBoneMatrixArraySRV,
					enWorldMatrixArraySRV,
					enNum
				};

				enum class EnDescHeapLayoutPerMaterial
				{
					enDiffuse,
					enNormal,
					enNum
				};

			public:
				CBasicModelRenderer(
					const nsRenderers::SModelInitData& modelInitData,
					bool isLODModel = false,
					std::shared_ptr<nsAnimations::CAnimator>* pAnimator = nullptr
				);
				~CBasicModelRenderer();

				void InitAfterImportScene(
					const nsRenderers::SModelInitData& modelInitData,
					const aiScene* scene
				);

				void InitAfterImportScene();

				void Release();

				void UpdateWorldMatrix(
					const nsMath::CVector3& position,
					const nsMath::CQuaternion& rotation,
					const nsMath::CVector3& scale
				) noexcept;

				void UpdateAnimation(float deltaTime, bool updateAnimMatrix) noexcept;

				inline void PlayAnimation(unsigned int animIdx) noexcept
				{
					if (m_animator)
					{
						m_animator->PlayAnimation(animIdx);
					}
				}

				inline void PlayAnimationFromBeginning(unsigned int animIdx) noexcept
				{
					if (m_animator)
					{
						m_animator->PlayAnimationFromBeginning(animIdx);
					}
				}

				inline void PlayAnimationFromMiddle(unsigned int animIdx, float timer) noexcept
				{
					if (m_animator)
					{
						m_animator->PlayAnimationFromMiddle(animIdx, timer);
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

				constexpr float GetAnimationSpeed() const noexcept
				{
					return m_animator ? m_animator->GetAnimationSpeed() : 1.0f;
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

				constexpr auto* GetAnimator() noexcept
				{
					return &m_animator;
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

				inline bool IsSkeltalAnimationValid() const noexcept
				{
					return m_animator && m_skelton;
				}

				constexpr EnLoadingState GetLoadingState() const noexcept
				{
					return m_loadingState;
				}

				void CheckLoaded() noexcept;

				bool InitAsynchronous(
					std::shared_ptr<nsAnimations::CAnimator>* pAnimator = nullptr) noexcept;

				constexpr void SetNumInstances(unsigned int numInstances) noexcept
				{
					m_numInstances = numInstances;
				}

				constexpr unsigned int GetNumInstances() const noexcept
				{
					return m_numInstances;
				}

				void UpdateWorldMatrixArray(const std::vector<nsMath::CMatrix>& worldMatrixArray);

				constexpr bool IsDrawingFlag() const noexcept
				{
					return m_drawingFlag;
				}

				constexpr void SetDrawingFlag(bool drawingFlag) noexcept
				{
					m_drawingFlag = drawingFlag;
					m_shadowModelRenderer.SetDrawingFlag(drawingFlag);
				} 

			private:
				bool Init(const nsRenderers::SModelInitData& modelInitData) noexcept;

				void Terminate() noexcept;

				bool InitSkeltalAnimation(
					const nsRenderers::SModelInitData& modelInitData,
					const aiScene* scene) noexcept;

				void InitMeshInfoArray(
					const aiScene* scene,
					const unsigned int numMeshes,
					unsigned int* numVerticesOut,
					unsigned int* numIndicesOut,
					std::vector<unsigned int>* baseVertexNoArrayOut = nullptr
				) noexcept;

				void LoadMeshes(
					const nsRenderers::SModelInitData& modelInitData,
					const aiScene* scene,
					std::vector<SMesh>* destMeshesOut,
					const unsigned int numMeshes
				) noexcept;

				void LoadMeshPerNode(
					const nsRenderers::SModelInitData& modelInitData,
					aiNode* node,
					const aiScene* scene,
					const nsMath::CMatrix& parentTransform,
					std::list<SMesh>* dstMeshesListOut
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
					std::vector<nsDx12Wrappers::CTexture*>* texturesOut
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

				void CreateDescriptorHeap();

				bool CreateModelCBV();

				bool CreateExpandCBV();

				bool CreateMaterialSRV();

				bool CreateBoneMatrisArraySB();

				bool CreateWorldMatrixArraySB(const nsRenderers::SModelInitData& modelInitData);

				bool CreateShadowMapSRV();

				void DrawShadowModel(nsDx12Wrappers::CCommandList* commandList);

				constexpr bool CheckIsDrawing() const noexcept
				{
					return m_loadingState == EnLoadingState::enAfterLoading &&
						m_fixNumInstanceOnFrame > 0 &&
						IsDrawingFlag();
				}

			private:
				std::vector<nsDx12Wrappers::CVertexBuffer*> m_vertexBuffers = {};
				std::vector<nsDx12Wrappers::CIndexBuffer*> m_indexBuffers = {};

				nsDx12Wrappers::CDescriptorHeap m_descriptorHeap = {};
				nsDx12Wrappers::CDescriptorHandle m_descHandlePerModel;
				nsDx12Wrappers::CDescriptorHandle m_descHandlePerMaterial;

				nsDx12Wrappers::CConstantBuffer m_modelCB = {};

				nsDx12Wrappers::CStructuredBuffer m_boneMatrixArraySB = {};
				nsDx12Wrappers::CStructuredBuffer m_worldMatrixArraySB = {};

				std::vector<nsDx12Wrappers::CTexture*> m_diffuseTextures = {};
				std::vector<nsDx12Wrappers::CTexture*> m_normalTextures = {};


				nsMath::CMatrix m_bias = nsMath::CMatrix::Identity();
				nsMath::CMatrix m_worldMatrix = nsMath::CMatrix::Identity();

				std::vector<SBasicMeshInfo> m_meshInfoArray = {};
				std::vector<nsMath::CMatrix> m_boneMatrices = {};
				nsAnimations::CSkelton* m_skelton = nullptr;
				std::shared_ptr<nsAnimations::CAnimator> m_animator = nullptr;

				EnLoadingState m_loadingState = EnLoadingState::enBeforeLoading;
				bool m_isImportedModelScene = false;
				Assimp::Importer* m_importerForLoadAsynchronous = nullptr;
				const aiScene* m_sceneForLoadAsynchronous = nullptr;

				const nsRenderers::SModelInitData* m_modelInitDataRef = nullptr;
				unsigned int m_numInstances = 1;

				std::vector<nsGeometries::CGeometryData*> m_geometryDataArray = {};
				unsigned int m_fixNumInstanceOnFrame = 0;

				nsShadow::CShadowModelRenderer m_shadowModelRenderer = {};

				bool m_isLODModel = false;
				bool m_drawingFlag = false;
			};

		}
	}
}