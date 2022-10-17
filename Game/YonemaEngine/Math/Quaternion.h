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
			constexpr explicit CQuaternion() noexcept;
			constexpr CQuaternion(float x, float y, float z, float w) noexcept;
			constexpr explicit CQuaternion(const CVector4& v) noexcept;

			inline ~CQuaternion() = default;

		public:
			inline CQuaternion& operator*=(const CQuaternion& q) noexcept;
			inline CQuaternion& operator=(const CVector4& v) noexcept;

		public:

			static inline const CQuaternion& Identity() noexcept;

			inline void SetRotation(const CVector3& axis, float radAngle) noexcept;
			void SetRotation(const CMatrix& mat) noexcept;
			void SetRotation(const CVector3& fromVec, const CVector3& toVec) noexcept;
			inline void SetRotationX(float radAngle) noexcept;
			inline void SetRotationY(float radAngle) noexcept;
			inline void SetRotationZ(float radAngle) noexcept;

			inline void SetRotationDeg(const CVector3& axis, float degAngle) noexcept;
			inline void SetRotationXDeg(float degAngle) noexcept;
			inline void SetRotationYDeg(float degAngle) noexcept;
			inline void SetRotationZDeg(float degAngle) noexcept;

			inline void SetRotationYFromDirectionXZ(const CVector3& direction) noexcept;

			inline void AddRotation(const CVector3& axis, float radAngle) noexcept;
			inline void AddRotationX(float radAngle) noexcept;
			inline void AddRotationY(float radAngle) noexcept;
			inline void AddRotationZ(float radAngle) noexcept;

			inline void AddRotationDeg(const CVector3& axis, float degAngle) noexcept;
			inline void AddRotationXDeg(float degAngle) noexcept;
			inline void AddRotationYDeg(float degAngle) noexcept;
			inline void AddRotationZDeg(float degAngle) noexcept;

			inline void Multiply(const CQuaternion& q) noexcept;
			inline void Multiply(const CQuaternion& q0, const CQuaternion& q1) noexcept;

			inline void Apply(CVector4& vOutput) const noexcept;
			inline void Apply(CVector3& vOutput) const noexcept;

			inline void Slerp(float t, const CQuaternion& q0, const CQuaternion& q1) noexcept;

		};

		static inline CQuaternion operator*(const CQuaternion& q0, const CQuaternion q1) noexcept;


		//////////////// 関数定義 ////////////////

		constexpr CQuaternion::CQuaternion() noexcept
			:CVector4(0.0f,0.0f,0.0f,1.0f)
		{
			return;
		}
		constexpr CQuaternion::CQuaternion(float x, float y, float z, float w) noexcept
			:CVector4(x, y, z, w)
		{
			return;
		}
		constexpr CQuaternion::CQuaternion(const CVector4& v) noexcept
			: CVector4(v)
		{
			return;
		}

		inline CQuaternion& CQuaternion::operator*=(const CQuaternion& q) noexcept
		{
			Multiply(q, *this);
			return *this;
		}
		inline CQuaternion& CQuaternion::operator=(const CVector4& v) noexcept
		{
			m_xmf4Vec = v.m_xmf4Vec;
			return *this;
		}

		inline const CQuaternion& CQuaternion::Identity() noexcept
		{
			return m_kIdentity;
		}


		inline void CQuaternion::SetRotation(const CVector3& axis, float radAngle) noexcept
		{
			auto xmv = DirectX::XMQuaternionRotationAxis(axis, radAngle);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::SetRotationX(float radAngle) noexcept
		{
			SetRotation(CVector3::AxisX(), radAngle);
			return;
		}
		inline void CQuaternion::SetRotationY(float radAngle) noexcept
		{
			SetRotation(CVector3::AxisY(), radAngle);
			return;
		}
		inline void CQuaternion::SetRotationZ(float radAngle) noexcept
		{
			SetRotation(CVector3::AxisZ(), radAngle);
			return;
		}


		inline void CQuaternion::SetRotationDeg(const CVector3& axis, float degAngle) noexcept
		{
			SetRotation(axis, DirectX::XMConvertToRadians(degAngle));
			return;
		}
		inline void CQuaternion::SetRotationXDeg(float degAngle) noexcept
		{
			SetRotationDeg(CVector3::AxisX(), degAngle);
			return;
		}
		inline void CQuaternion::SetRotationYDeg(float degAngle) noexcept
		{
			SetRotationDeg(CVector3::AxisY(), degAngle);
			return;
		}
		inline void CQuaternion::SetRotationZDeg(float degAngle) noexcept
		{
			SetRotationDeg(CVector3::AxisZ(), degAngle);
			return;
		}

		inline void CQuaternion::SetRotationYFromDirectionXZ(const CVector3& direction) noexcept
		{
			SetRotationY(atan2f(direction.x, direction.z));
			return;
		}

		inline void CQuaternion::AddRotation(const CVector3& axis, float radAngle) noexcept
		{
			CQuaternion addQ;
			addQ.SetRotation(axis, radAngle);
			*this *= addQ;
			return;
		}
		inline void CQuaternion::AddRotationX(float radAngle) noexcept
		{
			AddRotation(CVector3::AxisX(), radAngle);
			return;
		}
		inline void CQuaternion::AddRotationY(float radAngle) noexcept
		{
			AddRotation(CVector3::AxisY(), radAngle);
			return;
		}
		inline void CQuaternion::AddRotationZ(float radAngle) noexcept
		{
			AddRotation(CVector3::AxisZ(), radAngle);
			return;
		}

		inline void CQuaternion::AddRotationDeg(const CVector3& axis, float degAngle) noexcept
		{
			AddRotation(axis, DirectX::XMConvertToRadians(degAngle));
			return;
		}
		inline void CQuaternion::AddRotationXDeg(float degAngle) noexcept
		{
			AddRotationDeg(CVector3::AxisX(), degAngle);
			return;
		}
		inline void CQuaternion::AddRotationYDeg(float degAngle) noexcept
		{
			AddRotationDeg(CVector3::AxisY(), degAngle);
			return;
		}
		inline void CQuaternion::AddRotationZDeg(float degAngle) noexcept
		{
			AddRotationDeg(CVector3::AxisZ(), degAngle);
			return;
		}

		inline void CQuaternion::Multiply(const CQuaternion& q) noexcept
		{
			auto xmv = DirectX::XMQuaternionMultiply(*this, q);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::Multiply(const CQuaternion& q0, const CQuaternion& q1) noexcept
		{
			auto xmv = DirectX::XMQuaternionMultiply(q0, q1);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}

		inline void CQuaternion::Apply(CVector4& vOutput) const noexcept
		{
			auto xmv = DirectX::XMVector3Rotate(vOutput, *this);
			DirectX::XMStoreFloat4(&vOutput.m_xmf4Vec, xmv);
			return;
		}
		inline void CQuaternion::Apply(CVector3& vOutput) const noexcept
		{
			auto xmv = DirectX::XMVector3Rotate(vOutput, *this);
			DirectX::XMStoreFloat3(&vOutput.m_xmf3Vec, xmv);
			return;
		}

		inline void CQuaternion::Slerp(float t, const CQuaternion& q0, const CQuaternion& q1) noexcept
		{
			auto xmv0 = DirectX::XMLoadFloat4(&q0.m_xmf4Vec);
			auto xmv1 = DirectX::XMLoadFloat4(&q1.m_xmf4Vec);
			auto xmvr = DirectX::XMQuaternionSlerp(xmv0, xmv1, t);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmvr);
			return;
		}

		static inline CQuaternion operator*(const CQuaternion& q0, const CQuaternion q1) noexcept
		{
			CQuaternion qRet;
			qRet.Multiply(q1, q0);
			return qRet;
		}


	}
}