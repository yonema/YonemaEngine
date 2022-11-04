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
			static const CVector2 m_kCenter;

		public:
			/**
			 * @brief Ç∑Ç◊ÇƒÇÃóvëfÇ™0.0fÇ≈èâä˙âªÇ≥ÇÍÇ‹Ç∑ÅB
			*/
			constexpr explicit CVector2() noexcept;
			constexpr CVector2(float x, float y) noexcept;
			constexpr CVector2(const CVector2& v) noexcept;
			inline ~CVector2() = default;

		public:
			inline operator DirectX::XMVECTOR() const noexcept;

			inline CVector2& operator=(const CVector2& v) noexcept;
			inline CVector2& operator+=(const CVector2& v) noexcept;
			inline CVector2& operator-=(const CVector2& v) noexcept;
			inline CVector2& operator*=(float s) noexcept;
			inline CVector2& operator/=(float d) noexcept;

		public:
			inline static const CVector2& Zero() noexcept;
			inline static const CVector2& One() noexcept;
			inline static const CVector2& Right() noexcept;
			inline static const CVector2& Left() noexcept;
			inline static const CVector2& Up() noexcept;
			inline static const CVector2& Down() noexcept;
			inline static const CVector2& AxisX() noexcept;
			inline static const CVector2& AxisY() noexcept;
			inline static const CVector2& Center() noexcept;


			inline void Add(const CVector2& v) noexcept;
			inline void Add(const CVector2& v0, const CVector2& v1) noexcept;
			inline void Subtract(const CVector2& v) noexcept;
			inline void Subtract(const CVector2& v0, const CVector2& v1) noexcept;
			inline void Scale(float s) noexcept;
			inline void Div(float d) noexcept;
			inline float Dot(const CVector2& v) const noexcept;
			inline void Cross(const CVector2& v) noexcept;
			inline void Cross(const CVector2& v0, const CVector2& v1) noexcept;
			inline float Length() const noexcept;
			inline float LengthSq() const noexcept;
			inline void Max(const CVector2& v) noexcept;
			inline void Min(const CVector2& v) noexcept;
			inline void Lerp(float t, const CVector2& v0, const CVector2& v1) noexcept;
			inline void Normalize() noexcept;

		public:
			union
			{
				DirectX::XMFLOAT2 m_xmf2Vec;
				float m_fVec[2];
				struct { float x, y; };
			};
		};


		static inline CVector2 operator+(const CVector2& v0, const CVector2& v1) noexcept;
		static inline CVector2 operator-(const CVector2& v0, const CVector2& v1) noexcept;
		static inline CVector2 operator*(const CVector2& v, float s) noexcept;
		static inline CVector2 operator/(const CVector2& v, float d) noexcept;
		static inline float Dot(const CVector2& v0, const CVector2& v1) noexcept;
		static inline CVector2 Cross(const CVector2& v0, const CVector2 v1) noexcept;



		//////////////// ä÷êîíËã` ////////////////


		constexpr CVector2::CVector2() noexcept
			:x(0.0f), y(x)
		{
			return;
		}
		constexpr CVector2::CVector2(float x, float y) noexcept
			:x(x), y(y)
		{
			return;
		}
		constexpr CVector2::CVector2(const CVector2& v) noexcept
			:x(v.x), y(v.y)
		{
			return;
		}


		inline CVector2::operator DirectX::XMVECTOR() const noexcept
		{
			return DirectX::XMLoadFloat2(&m_xmf2Vec);
		}

		inline CVector2& CVector2::operator=(const CVector2& v) noexcept
		{
			m_xmf2Vec = v.m_xmf2Vec;
			return *this;
		}
		inline CVector2& CVector2::operator+=(const CVector2& v) noexcept
		{
			Add(v);
			return *this;
		}
		inline CVector2& CVector2::operator-=(const CVector2& v) noexcept
		{
			Subtract(v);
			return *this;
		}
		inline CVector2& CVector2::operator*=(float s) noexcept
		{
			Scale(s);
			return *this;
		}
		inline CVector2& CVector2::operator/=(float d) noexcept
		{
			Div(d);
			return *this;
		}


		inline const CVector2& CVector2::Zero() noexcept
		{
			return m_kZero;
		}
		inline const CVector2& CVector2::One() noexcept
		{
			return m_kOne;
		}
		inline const CVector2& CVector2::Right() noexcept
		{
			return m_kRight;
		}
		inline const CVector2& CVector2::Left() noexcept
		{
			return m_kLeft;
		}
		inline const CVector2& CVector2::Up() noexcept
		{
			return m_kUp;
		}
		inline const CVector2& CVector2::Down() noexcept
		{
			return m_kDown;
		}
		inline const CVector2& CVector2::AxisX() noexcept
		{
			return m_kAxisX;
		}
		inline const CVector2& CVector2::AxisY() noexcept
		{
			return m_kAxisY;
		}
		inline const CVector2& CVector2::Center() noexcept
		{
			return m_kCenter;
		}

		inline void CVector2::Add(const CVector2& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Add(const CVector2& v0, const CVector2& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Subtract(const CVector2& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Subtract(const CVector2& v0, const CVector2& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Scale(float s) noexcept
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmv);
			return;
		}
		inline void CVector2::Div(float d) noexcept
		{
			float scale = 1.0f / d;
			Scale(scale);
			return;
		}
		inline float CVector2::Dot(const CVector2& v) const noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			return DirectX::XMVector2Dot(xmv0, xmv1).m128_f32[0];
		}
		inline void CVector2::Cross(const CVector2& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			auto xmvr = DirectX::XMVector2Cross(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Cross(const CVector2& v0, const CVector2& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto xmvr = DirectX::XMVector2Cross(xmv0, xmv1);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline float CVector2::Length() const noexcept
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			return DirectX::XMVector2Length(xmv).m128_f32[0];
		}
		inline float CVector2::LengthSq() const noexcept
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			return DirectX::XMVector2LengthSq(xmv).m128_f32[0];
		}
		inline void CVector2::Max(const CVector2& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			DirectX::XMStoreFloat2(&m_xmf2Vec, DirectX::XMVectorMax(xmv0, xmv1));
		}
		inline void CVector2::Min(const CVector2& v) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v.m_xmf2Vec);
			DirectX::XMStoreFloat2(&m_xmf2Vec, DirectX::XMVectorMin(xmv0, xmv1));
			return;
		}
		inline void CVector2::Lerp(float t, const CVector2& v0, const CVector2& v1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat2(&v0.m_xmf2Vec);
			auto xmv1 = DirectX::XMLoadFloat2(&v1.m_xmf2Vec);
			auto  xmvr = DirectX::XMVectorLerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmvr);
			return;
		}
		inline void CVector2::Normalize() noexcept
		{
			auto xmv = DirectX::XMLoadFloat2(&m_xmf2Vec);
			xmv = DirectX::XMVector2Normalize(xmv);
			DirectX::XMStoreFloat2(&m_xmf2Vec, xmv);
		}



		inline CVector2 operator+(const CVector2& v0, const CVector2& v1) noexcept
		{
			CVector2 result(v0);
			result.Add(v1);
			return result;
		}
		inline CVector2 operator-(const CVector2& v0, const CVector2& v1) noexcept
		{
			CVector2 result(v0);
			result.Subtract(v1);
			return result;
		}
		inline CVector2 operator*(const CVector2& v, float s) noexcept
		{
			CVector2 result(v);
			result.Scale(s);
			return result;
		}
		inline CVector2 operator/(const CVector2& v, float d) noexcept
		{
			CVector2 result(v);
			result.Div(d);
			return result;
		}
		inline float Dot(const CVector2& v0, const CVector2& v1) noexcept
		{
			return v0.Dot(v1);
		}
		inline CVector2 Cross(const CVector2& v0, const CVector2 v1) noexcept
		{
			CVector2 result(v0);
			result.Cross(v1);
			return result;
		}
	}
}