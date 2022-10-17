#pragma once

namespace nsYMEngine
{
	namespace nsMath
	{
		class CVector3
		{
		private:
			static const CVector3 m_kZero;
			static const CVector3 m_kOne;
			static const CVector3 m_kRight;
			static const CVector3 m_kLeft;
			static const CVector3 m_kUp;
			static const CVector3 m_kDown;
			static const CVector3 m_kFront;
			static const CVector3 m_kBack;
			static const CVector3 m_kAxisX;
			static const CVector3 m_kAxisY;
			static const CVector3 m_kAxisZ;

		public:
			/**
			 * @brief Ç∑Ç◊ÇƒÇÃóvëfÇ™0.0fÇ≈èâä˙âªÇ≥ÇÍÇ‹Ç∑ÅB
			*/
			constexpr explicit CVector3() noexcept;
			constexpr CVector3(float x, float y, float z) noexcept;
			constexpr CVector3(const CVector3& v) noexcept;
			inline ~CVector3() = default;

		public:
			inline operator DirectX::XMVECTOR() const noexcept;

			inline CVector3& operator=(const CVector3& v) noexcept;
			inline CVector3& operator+=(const CVector3& v) noexcept;
			inline CVector3& operator-=(const CVector3& v) noexcept;
			inline CVector3& operator*=(float s) noexcept;
			inline CVector3& operator/=(float d) noexcept;

		public:
			inline static const CVector3& Zero() noexcept;
			inline static const CVector3& One() noexcept;
			inline static const CVector3& Right() noexcept;
			inline static const CVector3& Left() noexcept;
			inline static const CVector3& Up() noexcept;
			inline static const CVector3& Down() noexcept;
			inline static const CVector3& Front() noexcept;
			inline static const CVector3& Back() noexcept;
			inline static const CVector3& AxisX() noexcept;
			inline static const CVector3& AxisY() noexcept;
			inline static const CVector3& AxisZ() noexcept;

			inline void Add(const CVector3& v) noexcept;
			inline void Add(const CVector3& v0, const CVector3& v1) noexcept;
			inline void Subtract(const CVector3& v) noexcept;
			inline void Subtract(const CVector3& v0, const CVector3& v1) noexcept;
			inline void Scale(float s) noexcept;
			inline void Div(float d) noexcept;
			inline float Dot(const CVector3& v) const noexcept;
			inline void Cross(const CVector3& v) noexcept;
			inline void Cross(const CVector3& v0, const CVector3& v1) noexcept;
			inline float Length() const noexcept;
			inline float LengthSq() const noexcept;
			inline void Max(const CVector3& v) noexcept;
			inline void Min(const CVector3& v) noexcept;
			inline void Lerp(float t, const CVector3& v0, const CVector3& v1) noexcept;
			inline void Normalize() noexcept;

		public:
			union
			{
				DirectX::XMFLOAT3 m_xmf3Vec;
				float m_fVec[3];
				struct { float x, y, z; };
			};
		};


		static inline CVector3 operator+(const CVector3& v0, const CVector3& v1) noexcept;
		static inline CVector3 operator-(const CVector3& v0, const CVector3& v1) noexcept;
		static inline CVector3 operator*(const CVector3& v, float s) noexcept;
		static inline CVector3 operator/(const CVector3& v, float d) noexcept;
		static inline float Dot(const CVector3& v0, const CVector3& v1) noexcept;
		static inline CVector3 Cross(const CVector3& v0, const CVector3 v1) noexcept;


		//////////////// ä÷êîíËã` ////////////////


		constexpr CVector3::CVector3() noexcept
			:x(0.0f), y(x), z(y)
		{
			return;
		}
		constexpr CVector3::CVector3(float x, float y, float z) noexcept
			:x(x), y(y), z(z)
		{
			return;
		}
		constexpr CVector3::CVector3(const CVector3& v) noexcept
			: x(v.x), y(v.y), z(v.z)
		{
			return;
		}


		inline CVector3::operator DirectX::XMVECTOR() const noexcept
		{
			return DirectX::XMLoadFloat3(&m_xmf3Vec);
		}

		inline CVector3& CVector3::operator=(const CVector3& v) noexcept
		{
			m_xmf3Vec = v.m_xmf3Vec;
			return *this;
		}
		inline CVector3& CVector3::operator+=(const CVector3& v) noexcept
		{
			Add(v);
			return *this;
		}
		inline CVector3& CVector3::operator-=(const CVector3& v) noexcept
		{
			Subtract(v);
			return *this;
		}
		inline CVector3& CVector3::operator*=(float s) noexcept
		{
			Scale(s);
			return *this;
		}
		inline CVector3& CVector3::operator/=(float d) noexcept
		{
			Div(d);
			return *this;
		}


		inline const CVector3& CVector3::Zero() noexcept
		{
			return m_kZero;
		}
		inline const CVector3& CVector3::One() noexcept
		{
			return m_kOne;
		}
		inline const CVector3& CVector3::Right() noexcept
		{
			return m_kRight;
		}
		inline const CVector3& CVector3::Left() noexcept
		{
			return m_kLeft;
		}
		inline const CVector3& CVector3::Up() noexcept
		{
			return m_kUp;
		}
		inline const CVector3& CVector3::Down() noexcept
		{
			return m_kDown;
		}
		inline const CVector3& CVector3::Front() noexcept
		{
			return m_kFront;
		}
		inline const CVector3& CVector3::Back() noexcept
		{
			return m_kBack;
		}
		inline const CVector3& CVector3::AxisX() noexcept
		{
			return m_kAxisX;
		}
		inline const CVector3& CVector3::AxisY() noexcept
		{
			return m_kAxisY;
		}
		inline const CVector3& CVector3::AxisZ() noexcept
		{
			return m_kAxisZ;
		}


		inline void CVector3::Add(const CVector3& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Add(const CVector3& v0, const CVector3& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Subtract(const CVector3& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Subtract(const CVector3& v0, const CVector3& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Scale(float s) noexcept
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmv);
			return;
		}
		inline void CVector3::Div(float d) noexcept
		{
			float scale = 1.0f / d;
			Scale(scale);
			return;
		}
		inline float CVector3::Dot(const CVector3& v) const noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			return DirectX::XMVector3Dot(xmv0, xmv1).m128_f32[0];
		}
		inline void CVector3::Cross(const CVector3& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Cross(const CVector3& v0, const CVector3& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline float CVector3::Length() const noexcept
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			return DirectX::XMVector3Length(xmv).m128_f32[0];
		}
		inline float CVector3::LengthSq() const noexcept
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			return DirectX::XMVector3LengthSq(xmv).m128_f32[0];
		}
		inline void CVector3::Max(const CVector3& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			DirectX::XMStoreFloat3(&m_xmf3Vec, DirectX::XMVectorMax(xmv0, xmv1));
			return;
		}
		inline void CVector3::Min(const CVector3& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			DirectX::XMStoreFloat3(&m_xmf3Vec, DirectX::XMVectorMin(xmv0, xmv1));
			return;
		}
		inline void CVector3::Lerp(float t, const CVector3& v0, const CVector3& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto  xmvr = DirectX::XMVectorLerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Normalize() noexcept
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			xmv = DirectX::XMVector3Normalize(xmv);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmv);
			return;
		}


		inline CVector3 operator+(const CVector3& v0, const CVector3& v1) noexcept
		{
			CVector3 result(v0);
			result.Add(v1);
			return result;
		}
		inline CVector3 operator-(const CVector3& v0, const CVector3& v1) noexcept
		{
			CVector3 result(v0);
			result.Subtract(v1);
			return result;
		}
		inline CVector3 operator*(const CVector3& v, float s) noexcept
		{
			CVector3 result(v);
			result.Scale(s);
			return result;
		}
		inline CVector3 operator/(const CVector3& v, float d) noexcept
		{
			CVector3 result(v);
			result.Div(d);
			return result;
		}
		inline float Dot(const CVector3& v0, const CVector3& v1) noexcept
		{
			return v0.Dot(v1);
		}
		inline CVector3 Cross(const CVector3& v0, const CVector3 v1) noexcept
		{
			CVector3 result(v0);
			result.Cross(v1);
			return result;
		}





	}
}
