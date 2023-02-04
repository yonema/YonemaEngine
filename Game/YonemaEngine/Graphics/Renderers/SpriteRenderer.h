#pragma once
#include "../Sprites/Sprite.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{
			class CSpriteRenderer : public nsGameObject::IGameObject
			{
			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;

			public:
				constexpr CSpriteRenderer() = default;
				~CSpriteRenderer() = default;

				void Init(const nsSprites::SSpriteInitData& spriteInitData);

				/**
				 * @brief �A���J�[����̍��W��ݒ肵�܂��B
				 * ���W�n�� YDown, XRight �ł��B
				 * @param position ���W
				*/
				inline void SetPosition(const nsMath::CVector2& position) noexcept
				{
					m_position = position;
				}
				constexpr const nsMath::CVector2& GetPosition() const noexcept
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
				 * @brief �X�v���C�g�̊�_��ݒ肵�܂��B
				 * { 0.0f, 0.0f }������A
				 * { 0.5f, 0.5f }�������A
				 * { 1.0f, 1.0f }���E���ɂȂ�܂��B
				 * @param pivot ��_
				*/
				inline void SetPivot(const nsMath::CVector2& pivot)
				{
					m_pivot = pivot;
				}
				constexpr const nsMath::CVector2& GetPivot() const noexcept
				{
					return m_pivot;
				}
				constexpr void SetAnchor(EnAnchors anchor)
				{
					m_anchor = anchor;
				}
				constexpr EnAnchors GetAnchor() const noexcept
				{
					return m_anchor;
				}
				
				inline void SetMulColor(const nsMath::CVector4& mulColor) noexcept
				{
					if (m_sprite)
					{
						m_sprite->SetMulColor(mulColor);
					}
				}

				constexpr const nsMath::CVector4& GetMulColor() const noexcept
				{
					return m_sprite ? m_sprite->GetMulColor() : nsMath::CVector4::White();
				}

				constexpr bool IsDrawingFlag() const noexcept
				{
					return m_sprite ? m_sprite->IsDrawingFlag() : false;
				}

				constexpr void SetDrawingFlag(bool isDrawingFlag) noexcept
				{
					m_sprite ? m_sprite->SetDrawingFlag(isDrawingFlag) : static_cast<void>(this);
				}

			private:
				void Terminate();

			private:
				nsSprites::CSprite* m_sprite = nullptr;

				nsMath::CVector2 m_position = nsMath::CVector2::Zero();
				nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
				nsMath::CVector3 m_scale = nsMath::CVector3::One();
				nsMath::CVector2 m_pivot = nsMath::CVector2::Center();
				EnAnchors m_anchor = EnAnchors::enMiddleCenter;

			};

		}
	}
}