#pragma once
#include "RendererTable.h"
#include "../Models/BasicModelRenderer.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			struct SAnimationInitData;
		}
	}
	namespace nsPhysics
	{
		class IPhysicsObject;
		struct SMeshGeometryBuffer;
	}
}


namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			struct SModelInitData
			{
				const char* modelFilePath = nullptr;
				CRendererTable::EnRendererType rendererType = 
					CRendererTable::EnRendererType::enBasicModel;
				nsMath::CQuaternion vertexBias = nsMath::CQuaternion::Identity();
				const nsAnimations::SAnimationInitData* animInitData = nullptr;
				nsPhysics::SMeshGeometryBuffer* physicsMeshGeomBuffer = nullptr;
				unsigned int maxInstance = 1;
				const char* textureRootPath = nullptr;
				bool enableNodeTransform = false;
				bool enableLoadingAsynchronous = false;
			};

			class CModelRenderer : public nsGameObject::IGameObject
			{
			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;



			public:
				constexpr CModelRenderer() = default;
				~CModelRenderer() = default;

				void Init(const SModelInitData& modelInitData) noexcept;

				inline void SetPosition(const nsMath::CVector3& position) noexcept
				{
					m_position = position;
				}
				constexpr const nsMath::CVector3& GetPosition() const noexcept
				{
					return m_position;
				}

				inline void SetRotation(const nsMath::CQuaternion& rotation) noexcept
				{
					m_rotation = rotation;
				}
				constexpr const nsMath::CQuaternion& GetRotation() const noexcept
				{
					return m_rotation;
				}

				inline void SetScale(const nsMath::CVector3& scale) noexcept
				{
					m_scale = scale;
				}
				inline void SetScale(float scale) noexcept
				{
					SetScale({ scale, scale, scale });
				}
				constexpr const nsMath::CVector3& GetScale() const noexcept
				{
					return m_scale;
				}

				constexpr std::vector<nsMath::CMatrix>* GetWorldMatrixArrayRef() noexcept
				{
					return &m_worldMatrixArray;
				}

				void UpdateWorldMatrixArray() noexcept;

				/**
				 * @brief アニメーションを再生する。アニメーションが設定されていなかったら何もしません。
				 * @param animIdx 再生するアニメーションのインデックス。SAnimationInitDataで設定した順番で登録されています。
				 * @param animSpeed アニメーションの再生速度。
				 * @param isLoop アニメーションのループ再生を行うか?trueでループ再生を行います。
				*/
				constexpr void PlayAnimation(unsigned int animIdx, float animSpeed = -1.0f, bool isLoop = true) noexcept
				{
					if (m_renderer)
					{
						SetAnimationSpeed(animSpeed);
						SetIsAnimationLoop(isLoop);
						m_renderer->PlayAnimation(animIdx);
					}
				}

				constexpr void PlayAnimationFromBeginning(unsigned int animIdx, float animSpeed = -1, bool isLoop = true) noexcept
				{
					if (m_renderer)
					{
						SetAnimationSpeed(animSpeed);
						SetIsAnimationLoop(isLoop);
						m_renderer->PlayAnimationFromBeginning(animIdx);
					}
				}

				/**
				 * @brief アニメーションを再生する。アニメーションが設定されていなかったら何もしません。
				 * @param animIdx 再生するアニメーションのインデックス。SAnimationInitDataで設定した順番で登録されています。
				 * @param isLoop アニメーションのループ再生を行うか?trueでループ再生を行います
				 * @param animSpeed アニメーションの再生速度。
				*/
				constexpr void PlayAnimation(unsigned int animIdx, bool isLoop, float animSpeed = -1.0f) noexcept
				{
					if (m_renderer)
					{
						SetAnimationSpeed(animSpeed);
						SetIsAnimationLoop(isLoop);
						m_renderer->PlayAnimation(animIdx);
					}
				}

				/**
				 * @brief アニメーションが再生中か？を得る
				 * @return アニメーションが再生中か？
				 * @retval true アニメーション再生中
				 * @retval false アニメーションが再生されていない
				*/
				constexpr bool IsPlaying() const noexcept
				{
					return m_renderer ? m_renderer->IsPlaying() : false;
				}

				/**
				 * @brief アニメーションの再生速度を設定します。
				 * @param animSpeed アニメーションの再生速度
				*/
				constexpr void SetAnimationSpeed(float animSpeed) noexcept
				{
					if (m_renderer)
					{
						m_renderer->SetAnimationSpeed(animSpeed);
					}
				}

				/**
				 * @brief アニメーションのループ再生を行うかを設定します。
				 * @param isLoop ループ再生を行うか?trueでループ再生を行います。
				 * @return
				*/
				constexpr void SetIsAnimationLoop(bool isLoop) noexcept
				{
					if (m_renderer)
					{
						m_renderer->SetIsAnimationLoop(isLoop);
					}
				}

				const auto& GetRenderer() const noexcept
				{
					return m_renderer;
				}

				inline void ReserveAnimationEventFuncArray(unsigned int animIdx, unsigned int size)
				{
					if (m_renderer)
					{
						m_renderer->ReserveAnimationEventFuncArray(animIdx, size);
					}
				}

				inline void AddAnimationEventFunc(
					unsigned int animIdx,
					const std::function<void(void)>& animationEventFunc)
				{
					if (m_renderer)
					{
						m_renderer->AddAnimationEventFunc(animIdx, animationEventFunc);
					}
				}

				unsigned int FindBoneId(const std::string& boneName) const noexcept
				{
					return m_renderer ? 
						m_renderer->FindBoneId(boneName) : 
						nsAnimations::CSkelton::m_kNotFoundBoneID;
				}

				constexpr bool IsBoneFound(unsigned int boneId) const noexcept
				{
					return boneId != nsAnimations::CSkelton::m_kNotFoundBoneID;
				}

				constexpr inline const nsMath::CMatrix& GetBoneMatixMS(unsigned int boneId) const noexcept
				{
					return m_renderer ? 
						m_renderer->GetBoneMatrix(boneId) : nsMath::CMatrix::Identity();
				}

				inline const nsMath::CMatrix GetBoneMatixWS(
					unsigned int boneId) const noexcept
				{
					return GetBoneMatixMS(boneId) * GetWorldMatrix();
				}

				constexpr const nsMath::CMatrix& GetWorldMatrix() const noexcept
				{
					return m_renderer ? m_renderer->GetWorldMatrix() : nsMath::CMatrix::Identity();
				}

				constexpr bool IsLoadingAsynchronous() const noexcept
				{
					return m_enableLoadingAsynchronous;
				}


			private:
				void Terminate() noexcept;

				void CreateRenderer(const SModelInitData& modelInitData) noexcept;

				void UpdateWorldMatrix() noexcept;

			private:
				nsModels::CBasicModelRenderer* m_renderer = nullptr;
				CRendererTable::EnRendererType m_rendererType =
					CRendererTable::EnRendererType::enNumType;

				nsMath::CVector3 m_position = nsMath::CVector3::Zero();
				nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
				nsMath::CVector3 m_scale = nsMath::CVector3::One();
				std::vector<nsMath::CMatrix> m_worldMatrixArray = {};

				bool m_enableLoadingAsynchronous = false;

				SModelInitData m_modelInitData = {};
			};

		}
	}
}