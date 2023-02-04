#pragma once
#include "../Graphics/Animations2D/Animation2D.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class CAnimatedSpriteRenderer : public nsGameObject::IGameObject
			{
			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;

			public:
				 CAnimatedSpriteRenderer() = default;
				~CAnimatedSpriteRenderer() = default;

				void Init(const nsSprites::SSpriteInitData & spriteInitData);

				/**
				 * @brief �A���J�[����̍��W��ݒ肵�܂��B
				 * ���W�n�� YDown, XRight �ł��B
				 * @param position ���W
				*/
				inline void SetPosition(const nsMath::CVector2 & position) noexcept
				{
					m_position = position;
					m_animationDefaultPos = position;

					if (m_animations2D.count(m_playingAnimationName) == 1)
					{
						m_animations2D.at(m_playingAnimationName).SetDefaultPosition(position);
					}
				}
				constexpr const nsMath::CVector2& GetPosition() const noexcept
				{
					return m_position;
				}

				inline void SetRotation(const nsMath::CQuaternion & rotation) noexcept
				{
					m_rotation = rotation;
					m_animationDefaultRotation = rotation;
				}
				constexpr const nsMath::CQuaternion& GetRotation() const noexcept
				{
					return m_rotation;
				}

				inline void SetScale(const nsMath::CVector3 & scale) noexcept
				{
					m_scale = scale;
					m_animationDefaultScale = scale;
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
				 * @brief �X�v���C�g�̊�_��ݒ肵�܂��B
				 * { 0.0f, 0.0f }������A
				 * { 0.5f, 0.5f }�������A
				 * { 1.0f, 1.0f }���E���ɂȂ�܂��B
				 * @param pivot ��_
				*/
				inline void SetPivot(const nsMath::CVector2 & pivot)
				{
					m_pivot = pivot;
					m_animationDefaultPivot = pivot;
				}
				constexpr const nsMath::CVector2& GetPivot() const noexcept
				{
					return m_pivot;
				}

				inline void SetMulColor(const nsMath::CVector4 & mulColor) noexcept
				{
					if (m_sprite)
					{
						m_sprite->SetMulColor(mulColor);

						m_animationDefaultMulColor = mulColor;
					}
				}

				constexpr const nsMath::CVector4& GetMulColor() const noexcept
				{
					return m_sprite ? m_sprite->GetMulColor() : nsMath::CVector4::White();
				}

				/**
				 * @brief �A�j���[�V����������������
				 * @param fileName �A�j���[�V�����t�@�C��(.tda)�̃t�@�C����
				*/
				void InitAnimation(const char* fileName);

				/**
				 * @brief �A�j���[�V�������Đ�����
				 * @param playAnimName �Đ�����A�j���[�V������
				*/
				void PlayAnimation(std::string playAnimName);

				/**
				 * @brief �A�j���[�V�������Đ�����?
				 * @return �A�j���[�V�������Đ������ǂ���
				*/
				bool IsPlayingAnimation()
				{
					if (m_animations2D.count(m_playingAnimationName) != 1)
					{
						return false;
					}

					return m_animations2D.at(m_playingAnimationName).IsPlaying();
				}

				/**
				 * @brief �Đ����̃A�j���[�V��������C�ɍŏI�t���[���܂Ői�߂�
				*/
				void ForceEnd()
				{
					if (m_animations2D.count(m_playingAnimationName) != 1 || m_animations2D.at(m_playingAnimationName).IsPlaying() == false)
					{
						return;
					}
					
					m_animations2D.at(m_playingAnimationName).ForceEnd();

					nsGraphics::nsAnimations2D::SAnimation2DFrameData frameData = m_animations2D.at(m_playingAnimationName).GetFrameData();

					AnimFrameDataToSprite(frameData);
					//TODO:���Έړ��A��Έړ��ł����͈Ⴄ�B
					m_position = frameData.Position;
				}

				/**
				 * @brief �Đ����̃A�j���[�V�����̍Đ����L�����Z������B
				*/
				void Cancel()
				{
					if (m_animations2D.count(m_playingAnimationName) != 1 || m_animations2D.at(m_playingAnimationName).IsPlaying() == false)
					{
						return;
					}

					m_animations2D.at(m_playingAnimationName).Cancel();

					m_position = m_animationDefaultPos;
					m_rotation = m_animationDefaultRotation;
					m_scale = m_animationDefaultScale;
					m_pivot = m_animationDefaultPivot;
					SetMulColor(m_animationDefaultMulColor);
				}

				/**
				 * @brief �A�j���[�V�����C�x���g���̋�����o�^����
				 * @param animationName �o�^����A�j���[�V������
				 * @param eventName �o�^����C�x���g��
				 * @param eventFunc �o�^����C�x���g
				*/
				void RegisterAnimationEvent(std::string animationName, std::string eventName, std::function<void(const nsGraphics::nsAnimations2D::SAnimation2DFrameData&)> eventFunc)
				{
					if (m_animations2D.count(animationName) != 1)
					{
						return;
					}

					m_animations2D.at(animationName).RegisterEvent(eventName, eventFunc);
				}
			private:
				void Terminate();

				/**
				* @brief �A�j���[�V�����̃t���[���f�[�^���X�v���C�g�ɔ��f������
				* @param frameData ���f������t���[���f�[�^
				*/
				void AnimFrameDataToSprite(const nsGraphics::nsAnimations2D::SAnimation2DFrameData frameData)
				{
					nsMath::CQuaternion qRot;
					qRot.AddRotationZDeg(frameData.RotateDeg);
					m_rotation = qRot;
					m_scale = nsMath::CVector3(frameData.Scale.x, frameData.Scale.y, 1.0f);
					if (m_sprite)
					{
						m_sprite->SetMulColor(frameData.MulColor);
					}
					m_pivot = frameData.Pivot;
				}

			private:
				nsSprites::CSprite* m_sprite = nullptr;

				nsMath::CVector2 m_position = nsMath::CVector2::Zero();
				nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
				nsMath::CVector3 m_scale = nsMath::CVector3::One();
				nsMath::CVector2 m_pivot = nsMath::CVector2::Center();
				EnAnchors m_anchor = EnAnchors::enMiddleCenter;

				//�A�j���[�V�����p
				std::unordered_map<std::string,nsGraphics::nsAnimations2D::CAnimation2D> m_animations2D;	//�A�j���[�V�����̔z��
				std::string m_playingAnimationName = "";													//�Đ����̃A�j���[�V������
				nsMath::CVector2 m_animationDefaultPos = nsMath::CVector2::Zero();							//�A�j���[�V�����Đ��O�̍��W
				nsMath::CQuaternion m_animationDefaultRotation = nsMath::CQuaternion::Identity();			//�A�j���[�V�����̍Đ��O�̉�]�p�x
				nsMath::CVector3 m_animationDefaultScale = nsMath::CVector3::One();							//�A�j���[�V�����̍Đ��O�̊g�嗦
				nsMath::CVector4 m_animationDefaultMulColor = nsMath::CVector4::White();					//�A�j���[�V�����̍Đ��O�̏�Z�J���[
				nsMath::CVector2 m_animationDefaultPivot = nsMath::CVector2::Center();						//�A�j���[�V�����̍Đ��O�̃s�{�b�g


			};
		}
	}
}

