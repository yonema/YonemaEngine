#pragma once
namespace Effekseer
{
	template <typename T>
	class RefPtr;

	class Effect;

	using EffectRef = RefPtr<Effect>;
}

namespace nsYMEngine
{
	namespace nsEffect
	{
		class CEffectPlayer : public nsGameObject::IGameObject
		{
		public:
			bool Start() override final;

			void OnDestroy() override final;

			void Update(float deltaTime) override final;

		public:
			constexpr CEffectPlayer() = default;
			~CEffectPlayer() = default;

			bool Init(const wchar_t* filePath);

			void Play();

			bool IsPlaying() const;

			void Stop();

			void Pause();

			bool IsPaused() const;

			void SetSpeed(float speed);

			float GetSpeed() const;

			inline void SetPosition(const nsMath::CVector3& position) noexcept
			{
				m_position = position;
				m_isDirty = true;
			}
			constexpr const nsMath::CVector3& GetPosition() const noexcept
			{
				return m_position;
			}

			inline void SetRotation(const nsMath::CQuaternion& rotation) noexcept
			{
				m_rotation = rotation;
				m_isDirty = true;
			}
			constexpr const nsMath::CQuaternion& GetRotation() const noexcept
			{
				return m_rotation;
			}

			inline void SetScale(const nsMath::CVector3& scale) noexcept
			{
				m_scale = scale;
				m_isDirty = true;
			}
			inline void SetScale(float scale) noexcept
			{
				SetScale({ scale, scale, scale });
			}
			constexpr const nsMath::CVector3& GetScale() const noexcept
			{
				return m_scale;
			}

		private:
			void UpdateWorldMatrix();

		private:
			const Effekseer::EffectRef* m_effectRef = nullptr;
			int m_instanceHandle = -1;
			nsMath::CVector3 m_position = nsMath::CVector3::Zero();
			nsMath::CQuaternion m_rotation = nsMath::CQuaternion::Identity();
			nsMath::CVector3 m_scale = nsMath::CVector3::One();
			bool m_isDirty = false;
		};

	}
}