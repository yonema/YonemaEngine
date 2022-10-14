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
			constexpr explicit CVector3();
			constexpr CVector3(float x, float y, float z);
			constexpr CVector3(const CVector3& v);
			inline ~CVector3() = default;

		public:
			inline operator DirectX::XMVECTOR() const;

			inline CVector3& operator=(const CVector3& v);
			inline CVector3& operator+=(const CVector3& v);
			inline CVector3& operator-=(const CVector3& v);
			inline CVector3& operator*=(float s);
			inline CVector3& operator/=(float d);

		public:
			inline static const CVector3& Zero();
			inline static const CVector3& One();
			inline static const CVector3& Right();
			inline static const CVector3& Left();
			inline static const CVector3& Up();
			inline static const CVector3& Down();
			inline static const CVector3& Front();
			inline static const CVector3& Back();
			inline static const CVector3& AxisX();
			inline static const CVector3& AxisY();
			inline static const CVector3& AxisZ();

			inline void Add(const CVector3& v);
			inline void Add(const CVector3& v0, const CVector3& v1);
			inline void Subtract(const CVector3& v);
			inline void Subtract(const CVector3& v0, const CVector3& v1);
			inline void Scale(float s);
			inline void Div(float d);
			inline float Dot(const CVector3& v) const;
			inline void Cross(const CVector3& v);
			inline void Cross(const CVector3& v0, const CVector3& v1);
			inline float Length() const;
			inline float LengthSq() const;
			inline void Max(const CVector3& v);
			inline void Min(const CVector3& v);
			inline void Lerp(float t, const CVector3& v0, const CVector3& v1);
			inline void Normalize();

		public:
			union
			{
				DirectX::XMFLOAT3 m_xmf3Vec;
				float m_fVec[3];
				struct { float x, y, z; };
			};
		};


		static inline CVector3 operator+(const CVector3& v0, const CVector3& v1);
		static inline CVector3 operator-(const CVector3& v0, const CVector3& v1);
		static inline CVector3 operator*(const CVector3& v, float s);
		static inline CVector3 operator/(const CVector3& v, float d);
		static inline float Dot(const CVector3& v0, const CVector3& v1);
		static inline CVector3 Cross(const CVector3& v0, const CVector3 v1);


		//////////////// ä÷êîíËã` ////////////////


		constexpr CVector3::CVector3()
			:x(0.0f), y(x), z(y)
		{
			return;
		}
		constexpr CVector3::CVector3(float x, float y, float z)
			:x(x), y(y), z(z)
		{
			return;
		}
		constexpr CVector3::CVector3(const CVector3& v)
			: x(v.x), y(v.y), z(v.z)
		{
			return;
		}


		inline CVector3::operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat3(&m_xmf3Vec);
		}

		inline CVector3& CVector3::operator=(const CVector3& v)
		{
			m_xmf3Vec = v.m_xmf3Vec;
			return *this;
		}
		inline CVector3& CVector3::operator+=(const CVector3& v)
		{
			Add(v);
			return *this;
		}
		inline CVector3& CVector3::operator-=(const CVector3& v)
		{
			Subtract(v);
			return *this;
		}
		inline CVector3& CVector3::operator*=(float s)
		{
			Scale(s);
			return *this;
		}
		inline CVector3& CVector3::operator/=(float d)
		{
			Div(d);
			return *this;
		}


		inline const CVector3& CVector3::Zero()
		{
			return m_kZero;
		}
		inline const CVector3& CVector3::One()
		{
			return m_kOne;
		}
		inline const CVector3& CVector3::Right()
		{
			return m_kRight;
		}
		inline const CVector3& CVector3::Left()
		{
			return m_kLeft;
		}
		inline const CVector3& CVector3::Up()
		{
			return m_kUp;
		}
		inline const CVector3& CVector3::Down()
		{
			return m_kDown;
		}
		inline const CVector3& CVector3::Front()
		{
			return m_kFront;
		}
		inline const CVector3& CVector3::Back()
		{
			return m_kBack;
		}
		inline const CVector3& CVector3::AxisX()
		{
			return m_kAxisX;
		}
		inline const CVector3& CVector3::AxisY()
		{
			return m_kAxisY;
		}
		inline const CVector3& CVector3::AxisZ()
		{
			return m_kAxisZ;
		}


		inline void CVector3::Add(const CVector3& v)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Add(const CVector3& v0, const CVector3& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Subtract(const CVector3& v)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Subtract(const CVector3& v0, const CVector3& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Scale(float s)
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmv);
			return;
		}
		inline void CVector3::Div(float d)
		{
			float scale = 1.0f / d;
			Scale(scale);
			return;
		}
		inline float CVector3::Dot(const CVector3& v) const
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			return DirectX::XMVector3Dot(xmv0, xmv1).m128_f32[0];
		}
		inline void CVector3::Cross(const CVector3& v)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			auto xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Cross(const CVector3& v0, const CVector3& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline float CVector3::Length() const
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			return DirectX::XMVector3Length(xmv).m128_f32[0];
		}
		inline float CVector3::LengthSq() const
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			return DirectX::XMVector3LengthSq(xmv).m128_f32[0];
		}
		inline void CVector3::Max(const CVector3& v)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			DirectX::XMStoreFloat3(&m_xmf3Vec, DirectX::XMVectorMax(xmv0, xmv1));
			return;
		}
		inline void CVector3::Min(const CVector3& v)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v.m_xmf3Vec);
			DirectX::XMStoreFloat3(&m_xmf3Vec, DirectX::XMVectorMin(xmv0, xmv1));
			return;
		}
		inline void CVector3::Lerp(float t, const CVector3& v0, const CVector3& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat3(&v0.m_xmf3Vec);
			auto xmv1 = DirectX::XMLoadFloat3(&v1.m_xmf3Vec);
			auto  xmvr = DirectX::XMVectorLerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmvr);
			return;
		}
		inline void CVector3::Normalize()
		{
			auto xmv = DirectX::XMLoadFloat3(&m_xmf3Vec);
			xmv = DirectX::XMVector3Normalize(xmv);
			DirectX::XMStoreFloat3(&m_xmf3Vec, xmv);
			return;
		}


		inline CVector3 operator+(const CVector3& v0, const CVector3& v1)
		{
			CVector3 result(v0);
			result.Add(v1);
			return result;
		}
		inline CVector3 operator-(const CVector3& v0, const CVector3& v1)
		{
			CVector3 result(v0);
			result.Subtract(v1);
			return result;
		}
		inline CVector3 operator*(const CVector3& v, float s)
		{
			CVector3 result(v);
			result.Scale(s);
			return result;
		}
		inline CVector3 operator/(const CVector3& v, float d)
		{
			CVector3 result(v);
			result.Div(d);
			return result;
		}
		inline float Dot(const CVector3& v0, const CVector3& v1)
		{
			return v0.Dot(v1);
		}
		inline CVector3 Cross(const CVector3& v0, const CVector3 v1)
		{
			CVector3 result(v0);
			result.Cross(v1);
			return result;
		}





	}
}
