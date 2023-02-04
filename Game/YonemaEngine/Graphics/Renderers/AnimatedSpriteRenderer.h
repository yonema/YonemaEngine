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
				 * @brief アンカーからの座標を設定します。
				 * 座標系は YDown, XRight です。
				 * @param position 座標
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
				 * @brief スプライトの基点を設定します。
				 * { 0.0f, 0.0f }が左上、
				 * { 0.5f, 0.5f }が中央、
				 * { 1.0f, 1.0f }が右下になります。
				 * @param pivot 基点
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
				 * @brief アニメーションを初期化する
				 * @param fileName アニメーションファイル(.tda)のファイル名
				*/
				void InitAnimation(const char* fileName);

				/**
				 * @brief アニメーションを再生する
				 * @param playAnimName 再生するアニメーション名
				*/
				void PlayAnimation(std::string playAnimName);

				/**
				 * @brief アニメーションが再生中か?
				 * @return アニメーションが再生中かどうか
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
				 * @brief 再生中のアニメーションを一気に最終フレームまで進める
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
					//TODO:相対移動、絶対移動でここは違う。
					m_position = frameData.Position;
				}

				/**
				 * @brief 再生中のアニメーションの再生をキャンセルする。
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
				 * @brief アニメーションイベント時の挙動を登録する
				 * @param animationName 登録するアニメーション名
				 * @param eventName 登録するイベント名
				 * @param eventFunc 登録するイベント
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
				* @brief アニメーションのフレームデータをスプライトに反映させる
				* @param frameData 反映させるフレームデータ
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

				//アニメーション用
				std::unordered_map<std::string,nsGraphics::nsAnimations2D::CAnimation2D> m_animations2D;	//アニメーションの配列
				std::string m_playingAnimationName = "";													//再生中のアニメーション名
				nsMath::CVector2 m_animationDefaultPos = nsMath::CVector2::Zero();							//アニメーション再生前の座標
				nsMath::CQuaternion m_animationDefaultRotation = nsMath::CQuaternion::Identity();			//アニメーションの再生前の回転角度
				nsMath::CVector3 m_animationDefaultScale = nsMath::CVector3::One();							//アニメーションの再生前の拡大率
				nsMath::CVector4 m_animationDefaultMulColor = nsMath::CVector4::White();					//アニメーションの再生前の乗算カラー
				nsMath::CVector2 m_animationDefaultPivot = nsMath::CVector2::Center();						//アニメーションの再生前のピボット


			};
		}
	}
}

