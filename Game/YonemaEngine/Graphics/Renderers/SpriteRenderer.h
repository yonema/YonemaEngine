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
				 * @brief アンカーからの座標を設定します。
				 * 座標系は YDown, XRight です。
				 * @param position 座標
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
				 * @brief スプライトの基点を設定します。
				 * { 0.0f, 0.0f }が左上、
				 * { 0.5f, 0.5f }が中央、
				 * { 1.0f, 1.0f }が右下になります。
				 * @param pivot 基点
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