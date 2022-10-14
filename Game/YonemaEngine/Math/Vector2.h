#pragma once
namespace nsYMEngine
{
	namespace nsMath
	{
		class CVector2
		{
		private:
			static const CVector2 m_kZero;
			static const CVector2 m_kOne;
			static const CVector2 m_kRight;
			static const CVector2 m_kLeft;
			static const CVector2 m_kUp;
			static const CVector2 m_kDown;
			static const CVector2 m_kAxisX;
			static const CVector2 m_kAxisY;

		public:
			/**
			 * @brief Ç∑Ç◊ÇƒÇÃóvëfÇ™0.0fÇ≈èâä˙âªÇ≥ÇÍÇ‹Ç∑ÅB
			*/
			constexpr explicit CVector2();
			constexpr CVector2(float x, float y);
			constexpr CVector2(const CVector2& v);
			inline ~CVector2() = default;

		public:
			inline operator DirectX::XMVECTOR() const;

			inline CVector2& operator=(const CVector2& v);
			inline CVector2& operator+=(const CVector2& v);
			inline CVector2& operator-=(const CVector2& v);
			inline CVector2& operator*=(float s);
			inline CVector2& operator/=(float d);

		public:
			inline static const CVector2& Zero();
			inline static const CVector2& One();
			inline static const CVector2& Right();
			inline static const CVector2& Left();
			inline static const CVector2& Up();
			inline static const CVector2& Down();
			inline static const CVector2& AxisX();
			inline static const CVector2& AxisY();


			inline void Add(const CVector2& v);
			inline void Add(const CVector2& v0, const CVector2& v1);
			inline void Subtract(const CVector2& v);
			inline void Subtract(const CVector2& v0, const CVector2& v1);
			inline void Scale(float s);
			inline void Div(float d);
			inline float Dot(const CVector2& v) const;
			inline void Cross(const CVector2& v);
			inline void Cross(const CVector2& v0, const CVector2& v1);
			inline float Length() const;
			inline float LengthSq() const;
			inline void Max(const CVector2& v);
			inline void Min(const CVector2& v);
			inline void Lerp(float t, const CVector2& v0, const CVector2& v1);
			inline void Normalize();

		public:
			union
			{
				DirectX::XMFLOAT2 m_xmf2Vec;
				float m_fVec[2];
				struct { float x, y; };
			};
		};


		static inline CVector2 operator+(const CVector2& v0, const CVector2& v1);
		static inline CVector2 operator-(const CVector2& v0, const CVector2& v1);
		static inline CVector2 operator*(const CVector2& v, float s);
		static inline CVector2 operator/(const CVector2& v, float d);
		static inline float Dot(const CVector2& v0, const CVector2& v1);
		static inline CVector2 Cross(const CVector2& v0, const CVector2 v1);



		//////////////// ä÷êîíËã` ////////////////


		constexpr CVector2::CVector2()
			:x(0.0f), y(x)
		{
			return;
		}
		constexpr CVector2::CVector2(float x, float y)
			:x(x), y(y)
		{
			return;
		}
		constexpr CVector2::CVector2(const CVector2& v)
			:x(v.x), y(v.y)
		{
			return;
		}


		inline CVector2::operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat2(&m_xmf2Vec);
		}

		inline CVector2& CVector2::operator=(const CVector2& v)
		{
			m_xmf2Vec = v.m_xmf2Vec;
			return *this;
		}
		inline CVector2& CVector2::operator+=(const CVector2& v)
		{
			Add(v);
			return *this;
		}
		inline CVector2& CVector2::operator-=(const CVector2& v)
		{
			Subtract(v);
			return *this;
		}
		inline CVector2& CVector2::operator*=(float s)
		{
			Scale(s);
			return *this;
		}
		inline CVector2& CVector2::operator/=(float d)
		{
			Div(d);
			return *this;
		}


		inline const CVector2& CVector2::Zero()
		{
			return m_kZero;
		}
		inline const CVector2& CVector2::One()
		{
			return m_kOne;
		}
		inline const CVector2& CVector2::Right()
		{
			return m_kRight;
		}
		inline const CVector2& CVector2::Left()
		{
			return m_kLeft;
		}
		inline const CVector2& CVector2::Up()
		{
			return m_kUp;
		}
		inline const CVector2& CVector2::Down()
		{
			return m_kDown;
		}
		inline const CVector2& CVector2::AxisX()
		{
			return m_kAxisX;
		}
		inline const CVector2& CVector2::AxisY()
		{
			return m_kAxisY;
		}

		inline void CVector2::Add(const CVector2& v)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Add(const CVector2& v0, const CVector2& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Subtract(const CVector2& v)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Subtract(const CVector2& v0, const CVector2& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Scale(float s)
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmv);
			return;
		}
		inline void CVector2::Div(float d)
		{
			float scale = 1.0f / d;
			Scale(scale);
			return;
		}
		inline float CVector2::Dot(const CVector2& v) const
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			return DirectX::XMVector2Dot(xmv0, xmv1).m128_f32[0];
		}
		inline void CVector2::Cross(const CVector2& v)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVector2Cross(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Cross(const CVector2& v0, const CVector2& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVector2Cross(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline float CVector2::Length() const
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			return DirectX::XMVector2Length(xmv).m128_f32[0];
		}
		inline float CVector2::LengthSq() const
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			return DirectX::XMVector2LengthSq(xmv).m128_f32[0];
		}
		inline void CVector2::Max(const CVector2& v)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			DirectX::XMStoreFloat2(&m_xmf2Vec, DirectX::XMVectorMax(xmv0, xmv1));
		}
		inline void CVector2::Min(const CVector2& v)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			DirectX::XMStoreFloat2(&m_xmf2Vec, DirectX::XMVectorMin(xmv0, xmv1));
			return;
		}
		inline void CVector2::Lerp(float t, const CVector2& v0, const CVector2& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto  xmvr = DirectX::XMVectorLerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Normalize()
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			xmv = DirectX::XMVector2Normalize(xmv);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmv);
		}



		inline CVector2 operator+(const CVector2& v0, const CVector2& v1)
		{
			CVector2 result(v0);
			result.Add(v1);
			return result;
		}
		inline CVector2 operator-(const CVector2& v0, const CVector2& v1)
		{
			CVector2 result(v0);
			result.Subtract(v1);
			return result;
		}
		inline CVector2 operator*(const CVector2& v, float s)
		{
			CVector2 result(v);
			result.Scale(s);
			return result;
		}
		inline CVector2 operator/(const CVector2& v, float d)
		{
			CVector2 result(v);
			result.Div(d);
			return result;
		}
		inline float Dot(const CVector2& v0, const CVector2& v1)
		{
			return v0.Dot(v1);
		}
		inline CVector2 Cross(const CVector2& v0, const CVector2 v1)
		{
			CVector2 result(v0);
			result.Cross(v1);
			return result;
		}
	}
}