#pragma once
#include "RendererTable.h"
#include "../Renderers/ModelRendererBase.h"

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
			enum class EnModelFormat
			{
				enNone = -1,
				enPMD,
				enFBX,
				enVRM,
				enNumModelFormat
			};

			struct SModelInitData : private nsUtils::SNoncopyable
			{
				const char* modelFilePath = nullptr;
				const char* animFilePath = nullptr;
				EnModelFormat modelFormat = EnModelFormat::enNone;
				CRendererTable::EnRendererType rendererType = 
					CRendererTable::EnRendererType::enNone;
				nsMath::CQuaternion vertexBias = nsMath::CQuaternion::Identity();
				bool isVertesTranspos = false;
				const nsAnimations::SAnimationInitData* animInitData = nullptr;
				nsPhysics::SMeshGeometryBuffer* physicsMeshGeomBuffer = nullptr;
			};

			class CModelRenderer : public nsGameObject::IGameObject
			{
			private:
				static const char* 
					m_kModelFormatExtensions[static_cast<int>(EnModelFormat::enNumModelFormat)];

			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;



			public:
				constexpr CModelRenderer() = default;
				~CModelRenderer() = default;

				void Init(const SModelInitData& modelInitData);

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

				/**
				 * @brief �A�j���[�V�������Đ�����B�A�j���[�V�������ݒ肳��Ă��Ȃ������牽�����܂���B
				 * @param animIdx �Đ�����A�j���[�V�����̃C���f�b�N�X�BSAnimationInitData�Őݒ肵�����Ԃœo�^����Ă��܂��B
				 * @param animSpeed �A�j���[�V�����̍Đ����x�B
				 * @param isLoop �A�j���[�V�����̃��[�v�Đ����s����?true�Ń��[�v�Đ����s���܂��B
				*/
				inline void PlayAnimation(unsigned int animIdx, float animSpeed = -1.0f, bool isLoop = true) noexcept
				{
					if (m_renderer)
					{
						SetAnimationSpeed(animSpeed);
						SetIsAnimationLoop(isLoop);
						m_renderer->PlayAnimation(animIdx);
					}
				}

				/**
				 * @brief �A�j���[�V�������Đ�����B�A�j���[�V�������ݒ肳��Ă��Ȃ������牽�����܂���B
				 * @param animIdx �Đ�����A�j���[�V�����̃C���f�b�N�X�BSAnimationInitData�Őݒ肵�����Ԃœo�^����Ă��܂��B
				 * @param isLoop �A�j���[�V�����̃��[�v�Đ����s����?true�Ń��[�v�Đ����s���܂�
				 * @param animSpeed �A�j���[�V�����̍Đ����x�B
				*/
				inline void PlayAnimation(unsigned int animIdx, bool isLoop, float animSpeed = -1.0f) noexcept
				{
					if (m_renderer)
					{
						SetAnimationSpeed(animSpeed);
						SetIsAnimationLoop(isLoop);
						m_renderer->PlayAnimation(animIdx);
					}
				}

				/**
				 * @brief �A�j���[�V�������Đ������H�𓾂�
				 * @return �A�j���[�V�������Đ������H
				 * @retval true �A�j���[�V�����Đ���
				 * @retval false �A�j���[�V�������Đ�����Ă��Ȃ�
				*/
				inline bool IsPlaying() const noexcept
				{
					return m_renderer ? m_renderer->IsPlaying() : false;
				}

				/**
				 * @brief �A�j���[�V�����̍Đ����x��ݒ肵�܂��B
				 * @param animSpeed �A�j���[�V�����̍Đ����x
				*/
				inline void SetAnimationSpeed(float animSpeed) noexcept
				{
					if (m_renderer)
					{
						m_renderer->SetAnimationSpeed(animSpeed);
					}
				}

				/**
				 * @brief �A�j���[�V�����̃��[�v�Đ����s������ݒ肵�܂��B
				 * @param isLoop ���[�v�Đ����s����?true�Ń��[�v�Đ����s���܂��B
				 * @return
				*/
				inline void SetIsAnimationLoop(bool isLoop) noexcept
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

			private:
				void Terminate();

				void CreateRenderer(
					EnModelFormat* pModelFormat, const SModelInitData& modelInitData);

				void RegistToRendererTable(
					EnModelFormat modelFormat, const SModelInitData& modelInitData);

				EnModelFormat FindMatchExtension(const char* extension);

			private:
				IModelRendererBase* m_renderer = nullptr;
				CRendererTable::EnRendererType m_rendererType =
					CRendererTable::EnRendererType::enNumType;

				nsMath::CVector3 m_position = nsMath::CVector3::Zero();
				nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
				nsMath::CVector3 m_scale = nsMath::CVector3::One();
			};

		}
	}
}