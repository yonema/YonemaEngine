#pragma once
namespace nsYMEngine
{
	namespace nsMath
	{
		class CMatrix;
	}
}

namespace nsYMEngine
{
	namespace nsMath
	{
		/**
		 * @attention CVector4クラスを継承しているが、CVector4クラスで仮想デストラクタを
		 * 定義していないため、このクラスのデストラクタは呼ばれない。
		*/
		class CQuaternion : public CVector4
		{
		private:
			static const CQuaternion m_kIdentity;

		public:
			/**
			 * @brief x,y,zは0.0f、wは1.0fで初期化されます。
			*/
			constexpr explicit CQuaternion();
			constexpr CQuaternion(float x, float y, float z, float w);
			constexpr explicit CQuaternion(const CVector4& v);

			inline ~CQuaternion() = default;

		public:
			inline CQuaternion& operator*=(const CQuaternion& q);
			inline CQuaternion& operator=(const CVector4& v);

		public:

			static inline const CQuaternion& Identity();

			inline void SetRotation(const CVector3& axis, float radAngle);
			void SetRotation(const CMatrix& mat);
			void SetRotation(const CVector3& fromVec, const CVector3& toVec);
			inline void SetRotationX(float radAngle);
			inline void SetRotationY(float radAngle);
			inline void SetRotationZ(float radAngle);

			inline void SetRotationDeg(const CVector3& axis, float degAngle);
			inline void SetRotationXDeg(float degAngle);
			inline void SetRotationYDeg(float degAngle);
			inline void SetRotationZDeg(float degAngle);

			inline void SetRotationYFromDirectionXZ(const CVector3& direction);

			inline void AddRotation(const CVector3& axis, float radAngle);
			inline void AddRotationX(float radAngle);
			inline void AddRotationY(float radAngle);
			inline void AddRotationZ(float radAngle);

			inline void AddRotationDeg(const CVector3& axis, float degAngle);
			inline void AddRotationXDeg(float degAngle);
			inline void AddRotationYDeg(float degAngle);
			inline void AddRotationZDeg(float degAngle);

			inline void Multiply(const CQuaternion& q);
			inline void Multiply(const CQuaternion& q0, const CQuaternion& q1);

			inline void Apply(CVector4& vOutput) const;
			inline void Apply(CVector3& vOutput) const;

			inline void Slerp(float t, const CQuaternion& q0, const CQuaternion& q1);

		};

		static inline CQuaternion operator*(const CQuaternion& q0, const CQuaternion q1);


		//////////////// 関数定義 ////////////////

		constexpr CQuaternion::CQuaternion()
			:CVector4(0.0f,0.0f,0.0f,1.0f)
		{
			return;
		}
		constexpr CQuaternion::CQuaternion(float x, float y, float z, float w)
			:CVector4(x, y, z, w)
		{
			return;
		}
		constexpr CQuaternion::CQuaternion(const CVector4& v)
			: CVector4(v)
		{
			return;
		}

		inline CQuaternion& CQuaternion::operator*=(const CQuaternion& q)
		{
			Multiply(q, *this);
			return *this;
		}
		inline CQuaternion& CQuaternion::operator=(const CVector4& v)
		{
			m_xmf4Vec = v.m_xmf4Vec;
			return *this;
		}

		inline const CQuaternion& CQuaternion::Identity()
		{
			return m_kIdentity;
		}


		inline void CQuaternion::SetRotation(const CVector3& axis, float radAngle)
		{
			auto xmv = DirectX::XMQuaternionRotationAxis(axis, radAngle);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::SetRotationX(float radAngle)
		{
			SetRotation(CVector3::AxisX(), radAngle);
			return;
		}
		inline void CQuaternion::SetRotationY(float radAngle)
		{
			SetRotation(CVector3::AxisY(), radAngle);
			return;
		}
		inline void CQuaternion::SetRotationZ(float radAngle)
		{
			SetRotation(CVector3::AxisZ(), radAngle);
			return;
		}


		inline void CQuaternion::SetRotationDeg(const CVector3& axis, float degAngle)
		{
			SetRotation(axis, DirectX::XMConvertToRadians(degAngle));
			return;
		}
		inline void CQuaternion::SetRotationXDeg(float degAngle)
		{
			SetRotationDeg(CVector3::AxisX(), degAngle);
			return;
		}
		inline void CQuaternion::SetRotationYDeg(float degAngle)
		{
			SetRotationDeg(CVector3::AxisY(), degAngle);
			return;
		}
		inline void CQuaternion::SetRotationZDeg(float degAngle)
		{
			SetRotationDeg(CVector3::AxisZ(), degAngle);
			return;
		}

		inline void CQuaternion::SetRotationYFromDirectionXZ(const CVector3& direction)
		{
			SetRotationY(atan2f(direction.x, direction.z));
			return;
		}

		inline void CQuaternion::AddRotation(const CVector3& axis, float radAngle)
		{
			CQuaternion addQ;
			addQ.SetRotation(axis, radAngle);
			*this *= addQ;
			return;
		}
		inline void CQuaternion::AddRotationX(float radAngle)
		{
			AddRotation(CVector3::AxisX(), radAngle);
			return;
		}
		inline void CQuaternion::AddRotationY(float radAngle)
		{
			AddRotation(CVector3::AxisY(), radAngle);
			return;
		}
		inline void CQuaternion::AddRotationZ(float radAngle)
		{
			AddRotation(CVector3::AxisZ(), radAngle);
			return;
		}

		inline void CQuaternion::AddRotationDeg(const CVector3& axis, float degAngle)
		{
			AddRotation(axis, DirectX::XMConvertToRadians(degAngle));
			return;
		}
		inline void CQuaternion::AddRotationXDeg(float degAngle)
		{
			AddRotationDeg(CVector3::AxisX(), degAngle);
			return;
		}
		inline void CQuaternion::AddRotationYDeg(float degAngle)
		{
			AddRotationDeg(CVector3::AxisY(), degAngle);
			return;
		}
		inline void CQuaternion::AddRotationZDeg(float degAngle)
		{
			AddRotationDeg(CVector3::AxisZ(), degAngle);
			return;
		}

		inline void CQuaternion::Multiply(const CQuaternion& q)
		{
			auto xmv = DirectX::XMQuaternionMultiply(*this, q);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::Multiply(const CQuaternion& q0, const CQuaternion& q1)
		{
			auto xmv = DirectX::XMQuaternionMultiply(q0, q1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}

		inline void CQuaternion::Apply(CVector4& vOutput) const
		{
			auto xmv = DirectX::XMVector3Rotate(vOutput, *this);
			DirectX::XMStoreFloat4(&vOutput.m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::Apply(CVector3& vOutput) const
		{
			auto xmv = DirectX::XMVector3Rotate(vOutput, *this);
			DirectX::XMStoreFloat3(&vOutput.m_xmf3Vec, xmv);
			return;
		}

		inline void CQuaternion::Slerp(float t, const CQuaternion& q0, const CQuaternion& q1)
		{
			auto xmv0 = DirectX::XMLoadFloat4(&q0.m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&q1.m_xmf4Vec);
			auto xmvr = DirectX::XMQuaternionSlerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}

		static inline CQuaternion operator*(const CQuaternion& q0, const CQuaternion q1)
		{
			CQuaternion qRet;
			qRet.Multiply(q1, q0);
			return qRet;
		}


	}
}