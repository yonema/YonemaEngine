#pragma once

namespace nsYMEngine
{
	namespace nsMath
	{

		class CVector4
		{
		private:
			static const CVector4 m_kIdentity;
			static const CVector4 m_kWhite;
			static const CVector4 m_kBlack;
			static const CVector4 m_kRed;
			static const CVector4 m_kGreen;
			static const CVector4 m_kBlue;
			static const CVector4 m_kGray;

		public:
			/**
			 * @brief x,y,zは0.0f、wは1.0fで初期化されます。
			*/
			constexpr explicit CVector4();
			constexpr CVector4(float x, float y, float z, float w);
			constexpr CVector4(const CVector4& v);
			/**
			 * @brief 3次元ベクトルを元にベクトルを初期化します。wは1.0fで初期化されます。
			*/
			constexpr explicit CVector4(const CVector3& v);

			/**
			 * @brief 基底クラスとして使われるけど、サイズがクラスのサイズが大きくなると
			 * 困るから仮想デストラクタにしない。
			 * 派生クラスのデストラクタはあきらめてもらう。
			*/
			inline ~CVector4() = default;
			//virtual inline ~CVector4() = default;

		public:
			inline operator DirectX::XMVECTOR() const;

			inline CVector4& operator=(const CVector4& v);
			inline CVector4& operator+=(const CVector4& v);
			inline CVector4& operator-=(const CVector4& v);
			inline CVector4& operator*=(float s);
			inline CVector4& operator/=(float d);

		public:

			static inline const CVector4& Identity();
			static inline const CVector4& White();
			static inline const CVector4& Black();
			static inline const CVector4& Red();
			static inline const CVector4& Green();
			static inline const CVector4& Blue();
			static inline const CVector4& Gray();

			inline void Add(const CVector4& v);
			inline void Add(const CVector4& v0, const CVector4 v1);
			inline void Subtract(const CVector4& v);
			inline void Subtract(const CVector4& v0, const CVector4 v1);
			inline void Scale(float s);
			inline void Div(float d);
			inline float Dot(const CVector4& v) const;
			inline float Length() const;
			inline float LengthSq() const;
			inline void Max(const CVector4& v);
			inline void Min(const CVector4& v);
			inline void Lerp(float t, const CVector4& v0, const CVector4& v1);
			inline void Normalize();


		public:
			union
			{
				DirectX::XMFLOAT4 m_xmf4Vec;
				struct { float x, y, z, w; };
				struct { float r, g, b, a; };
				float m_fVec[4];
			};

		};


		static inline CVector4 operator+(const CVector4& v0, const CVector4& v1);
		static inline CVector4 operator-(const CVector4& v0, const CVector4& v1);
		static inline CVector4 operator*(const CVector4& v, float s);
		static inline CVector4 operator/(const CVector4& v, float d);
		static inline float Dot(const CVector4& v0, const CVector4& v1);



		//////////////// 関数定義 ////////////////


		constexpr CVector4::CVector4()
			:x(0.0f),y(x),z(y),w(1.0f)
		{
			return;
		}
		constexpr CVector4::CVector4(float x, float y, float z, float w)
			:x(x), y(y), z(z), w(w)
		{
			return;
		}
		constexpr CVector4::CVector4(const CVector4& v)
			:x(v.x), y(v.y), z(v.z), w(v.w)
		{
			return;
		}
		constexpr CVector4::CVector4(const CVector3& v)
			: x(v.x), y(v.y), z(v.z), w(1.0f)
		{
			return;
		}



		inline CVector4::operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat4(&m_xmf4Vec);
		}
		inline CVector4& CVector4::operator=(const CVector4& v)
		{
			m_xmf4Vec = v.m_xmf4Vec;
			return *this;
		}
		inline CVector4& CVector4::operator+=(const CVector4& v)
		{
			Add(v);
			return *this;
		}
		inline CVector4& CVector4::operator-=(const CVector4& v)
		{
			Subtract(v);
			return *this;
		}
		inline CVector4& CVector4::operator*=(float s)
		{
			Scale(s);
			return *this;
		}
		inline CVector4& CVector4::operator/=(float d)
		{
			Div(d);
			return *this;
		}



		inline const CVector4& CVector4::Identity()
		{
			return m_kIdentity;
		}
		inline const CVector4& CVector4::White()
		{
			return m_kWhite;
		}
		inline const CVector4& CVector4::Black()
		{
			return m_kBlack;
		}
		inline const CVector4& CVector4::Red()
		{
			return m_kRed;
		}
		inline const CVector4& CVector4::Green()
		{
			return m_kGreen;
		}
		inline const CVector4& CVector4::Blue()
		{
			return m_kBlue;
		}
		inline const CVector4& CVector4::Gray()
		{
			return m_kGray;
		}


		inline void CVector4::Add(const CVector4& v)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v.m_xmf4Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}
		inline void CVector4::Add(const CVector4& v0, const CVector4 v1)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&v0.m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v1.m_xmf4Vec);
			auto xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}
		inline void CVector4::Subtract(const CVector4& v)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v.m_xmf4Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}
		inline void CVector4::Subtract(const CVector4& v0, const CVector4 v1)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&v0.m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v1.m_xmf4Vec);
			auto xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}
		inline void CVector4::Scale(float s)
		{
			auto xmv = DirectX::XMLoadFloat4(&m_xmf4Vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}
		inline void CVector4::Div(float d)
		{
			if (d == 0.0f)
			{
				return;
			}
			float scale = 1.0f / d;
			Scale(scale);
			return;
		}
		inline float CVector4::Dot(const CVector4& v) const
		{
			auto xmv0 = DirectX::XMLoadFloat4(&m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v.m_xmf4Vec);
			return DirectX::XMVector4Dot(xmv0, xmv1).m128_f32[0];
		}
		inline float CVector4::Length() const
		{
			auto xmv = DirectX::XMLoadFloat4(&m_xmf4Vec);
			return DirectX::XMVector4Length(xmv).m128_f32[0];
		}
		inline float CVector4::LengthSq() const
		{
			auto xmv = DirectX::XMLoadFloat4(&m_xmf4Vec);
			return DirectX::XMVector4LengthSq(xmv).m128_f32[0];
		}
		inline void CVector4::Max(const CVector4& v)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v.m_xmf4Vec);
			DirectX::XMStoreFloat4(&m_xmf4Vec, DirectX::XMVectorMax(xmv0, xmv1));
			return;
		}
		inline void CVector4::Min(const CVector4& v)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v.m_xmf4Vec);
			DirectX::XMStoreFloat4(&m_xmf4Vec, DirectX::XMVectorMin(xmv0, xmv1));
			return;
		}
		inline void CVector4::Lerp(float t, const CVector4& v0, const CVector4& v1)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&v0.m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&v1.m_xmf4Vec);
			auto xmvr = DirectX::XMVectorLerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}
		inline void CVector4::Normalize()
		{
			auto xmv = DirectX::XMLoadFloat4(&m_xmf4Vec);
			xmv = DirectX::XMVector4Normalize(xmv);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}




		inline CVector4 operator+(const CVector4& v0, const CVector4& v1)
		{
			CVector4 result(v0);
			result.Add(v1);
			return result;
		}
		inline CVector4 operator-(const CVector4& v0, const CVector4& v1)
		{
			CVector4 result(v0);
			result.Subtract(v1);
			return result;
		}
		inline CVector4 operator*(const CVector4& v, float s)
		{
			CVector4 result(v);
			result.Scale(s);
			return result;
		}
		inline CVector4 operator/(const CVector4& v, float d)
		{
			CVector4 result(v);
			result.Div(d);
			return result;
		}
		inline float Dot(const CVector4& v0, const CVector4& v1)
		{
			return v0.Dot(v1);
		}








	}
}