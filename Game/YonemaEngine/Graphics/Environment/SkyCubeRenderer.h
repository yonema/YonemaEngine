#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsEnvironment
		{
			enum class EnSkyType
			{
				enNormal,
				enNum
			};

			class CSkyCubeRenderer : public nsGameObject::IGameObject
			{
			private:
				static const char* const m_kModelFilePath[static_cast<int>(EnSkyType::enNum)];
				static const float m_kDefaultSkyScale;
				static const float m_kDefaultAutoRotateSpeed;

				struct SConstantBufferData
				{
					constexpr SConstantBufferData() = default;
					~SConstantBufferData() = default;

					float luminance = 1.0f;
				};

			public:
				bool Start() override final;

				void Update(float deltaTime) override final;

				void OnDestroy() override final;

			public:
				constexpr CSkyCubeRenderer() = default;
				~CSkyCubeRenderer() = default;

				void Init(EnSkyType skyType);

				inline void SetPosition(const nsMath::CVector3& position) noexcept
				{
					if (m_modelRenderer)
					{
						m_modelRenderer->SetPosition(position);
					}
				}
				constexpr const auto& GetPosition() const noexcept
				{
					return m_modelRenderer ? 
						m_modelRenderer->GetPosition() : nsMath::CVector3::Zero();
				}

				inline void SetRotation(const nsMath::CQuaternion& rotation) noexcept
				{
					if (m_modelRenderer)
					{
						m_modelRenderer->SetRotation(rotation);
					}
				}
				constexpr const auto& GetRotation() const noexcept
				{
					return m_modelRenderer ? 
						m_modelRenderer->GetRotation() : nsMath::CQuaternion::Identity();
				}
				inline void SetScale(float scale) noexcept
				{
					SetScale({ scale, scale, scale });
				}
				inline void SetScale(const nsMath::CVector3& scale) noexcept
				{
					if (m_modelRenderer)
					{
						m_modelRenderer->SetScale(scale);
					}
				}
				constexpr auto& GetScale() const noexcept
				{
					return m_modelRenderer ? m_modelRenderer->GetScale() : nsMath::CVector3::One();
				}

				inline void SetLuminance(float luminance) noexcept
				{
					m_constantBufferData.luminance = luminance;
					m_isDirtyFlag = true;
				}
				constexpr float GetLuminance() const noexcept
				{
					return m_constantBufferData.luminance;
				}

				constexpr float GetDefaultSclae() const noexcept
				{
					return m_kDefaultSkyScale;
				}

				constexpr void SetAutoFollowCameraFlag(bool flag) noexcept
				{
					m_autoFollowCameraFlag = flag;
				}
				constexpr bool GetAutoFollowCameraFlag()  const noexcept
				{
					return m_autoFollowCameraFlag;
				}

				constexpr void SetAutoRotateFlag(bool flag) noexcept
				{
					m_autoRotateFlag = flag;
				}
				constexpr bool GetAutoRotateFlag() const noexcept
				{
					return m_autoRotateFlag;
				}

				/**
				 * @brief スカイキューブが自動で回転する速度を設定する。
				 * m_autoRotateFlagがtrueの時のみ有効。
				 * speedの値が正と負で回転向きが反転する。
				 * @param speed 回転速度
				*/
				constexpr void SetAutoRotateSpeed(float speed) noexcept
				{
					m_autoRotateSpeed = speed;
				}
				constexpr float GetAutoRotateSpeed() const noexcept
				{
					return m_autoRotateSpeed;
				}

				constexpr float GetDefaultAutoRotateSpeed() const noexcept
				{
					return m_kDefaultAutoRotateSpeed;
				}

			private:
				void CreateExpandCB();

				void UpdateConstantBuffer() noexcept;

				void AutoFollowCamera() noexcept;

				void AutoRotate(float deltaTime) noexcept;

			private:
				nsRenderers::CModelRenderer* m_modelRenderer = nullptr;
				nsDx12Wrappers::CConstantBuffer m_expandConstantBuffer = {};
				SConstantBufferData m_constantBufferData = {};
				bool m_isDirtyFlag = false;
				bool m_autoFollowCameraFlag = false;
				bool m_autoRotateFlag = false;
				float m_autoRotateSpeed = m_kDefaultAutoRotateSpeed;
				float m_rotateDegAngle = 0.0f;
			};

		}
	}
}