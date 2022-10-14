#pragma once

namespace nsYMEngine
{
	namespace nsMath
	{
		class CMatrix
		{
		private:
			static const CMatrix m_kIdentity;
			static const CMatrix m_kZero;

		public:
			/**
			 * @brief 単位行列として初期化されます。
			*/
			constexpr explicit CMatrix();
			constexpr CMatrix(
				float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33
			);
			constexpr CMatrix(const CMatrix& m);
			constexpr explicit CMatrix(const DirectX::XMFLOAT4X4& m);

			inline ~CMatrix() = default;

		public:
			inline operator DirectX::XMMATRIX() const;

			inline CMatrix& operator=(const CMatrix& m);
			inline CMatrix& operator*=(const CMatrix& m);

		public:
			static inline const CMatrix& Identity();
			static inline const CMatrix& Zero();

			inline void MakeTranslation(const CVector3& offset);
			inline void MakeTranslation(float x, float y, float z);
			inline void MakeRotationX(float radAngle);
			inline void MakeRotationY(float radAngle);
			inline void MakeRotationZ(float radAngle);
			inline void MakeRotationAxis(const CVector3& axis, float radAngle);
			inline void MakeRotationFromQuaternion(const CQuaternion& q);
			inline void MakeScaling(const CVector3& scale);
			inline void MakeScaling(float x, float y, float z);

			inline void MakeProjectionMatrix(
				float fovAngleY, float aspectRatio, float nearZ, float farZ);
			inline void MakeOrthoProjectionMatrix(
				float viewWidth, float viewHeight, float nearZ, float farZ);
			inline void MakeViewMatrix(
				const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir);

			inline void Multiply(const CMatrix& m);
			inline void Multiply(const CMatrix& m0, const CMatrix& m1);

			inline void Inverse(const CMatrix& m);
			inline void Inverse();
			inline void Transpose();

			inline void Apply(CVector3& vOut) const;
			inline void Apply(CVector4& vOut) const;




		public:
			union
			{
				DirectX::XMFLOAT4X4 m_xmf4x4Mat;
				CVector4 m_vec4Mat[4];
				float m_fMat[4][4];
				struct {
					float _11, _12, _13, _14;
					float _21, _22, _23, _24;
					float _31, _32, _33, _34;
					float _41, _42, _43, _44;
				};
			};

		};


		static inline CMatrix operator*(const CMatrix& m0, const CMatrix& m1);


		//////////////// 関数定義 ////////////////


		constexpr CMatrix::CMatrix()
			:m_xmf4x4Mat(
				1.0f,0.0f,0.0f,0.0f,
				0.0f,1.0f,0.0f,0.0f,
				0.0f,0.0f,1.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f)
		{
			return;
		}
		constexpr CMatrix::CMatrix(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		)
			:m_xmf4x4Mat(
				m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33
			)
		{
			return;
		}
		constexpr CMatrix::CMatrix(const CMatrix& m)
			:m_xmf4x4Mat(m.m_xmf4x4Mat)
		{
			return;
		}
		constexpr CMatrix::CMatrix(const DirectX::XMFLOAT4X4& m)
			:m_xmf4x4Mat(m)
		{
			return;
		}


		inline CMatrix::operator DirectX::XMMATRIX() const
		{
			return DirectX::XMLoadFloat4x4(&m_xmf4x4Mat);
		}

		inline CMatrix& CMatrix::operator=(const CMatrix& m)
		{
			m_xmf4x4Mat = m.m_xmf4x4Mat;
			return *this;
		}
		inline CMatrix& CMatrix::operator*=(const CMatrix& m)
		{
			Multiply(m);
			return *this;
		}


		inline const CMatrix& CMatrix::Identity()
		{
			return m_kIdentity;
		}
		inline const CMatrix& CMatrix::Zero()
		{
			return m_kZero;
		}


		inline void CMatrix::MakeTranslation(const CVector3& offset)
		{
			auto xmmTrans = DirectX::XMMatrixTranslationFromVector(offset);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmTrans);
			return;
		}
		inline void CMatrix::MakeTranslation(float x, float y, float z)
		{
			MakeTranslation(CVector3(x, y, z));
			return;
		}
		inline void CMatrix::MakeRotationX(float radAngle)
		{
			auto xmmRot = DirectX::XMMatrixRotationX(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationY(float radAngle)
		{
			auto xmmRot = DirectX::XMMatrixRotationY(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationZ(float radAngle)
		{
			auto xmmRot = DirectX::XMMatrixRotationZ(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationAxis(const CVector3& axis, float radAngle)
		{
			auto xmmRot = DirectX::XMMatrixRotationAxis(axis, radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationFromQuaternion(const CQuaternion& q)
		{
			auto xmmRot = DirectX::XMMatrixRotationQuaternion(q);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeScaling(const CVector3& scale)
		{
			auto xmmScale = DirectX::XMMatrixScalingFromVector(scale);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmScale);
			return;
		}
		inline void CMatrix::MakeScaling(float x, float y, float z)
		{
			MakeScaling(CVector3(x, y, z));
			return;
		}


		inline void CMatrix::MakeProjectionMatrix(
			float fovAngleY, float aspectRatio, float nearZ, float farZ)
		{
			auto xmmProj =
				DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmProj);
			return;
		}
		inline void CMatrix::MakeOrthoProjectionMatrix(
			float viewWidth, float viewHeight, float nearZ, float farZ)
		{
			auto xmmProj =
				DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmProj);
			return;
		}
		inline void CMatrix::MakeViewMatrix(
			const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir)
		{
			auto xmmView =
				DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmView);
			return;
		}

		inline void CMatrix::Multiply(const CMatrix& m)
		{
			auto xmm = DirectX::XMMatrixMultiply(*this, m);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmm);
			return;
		}
		inline void CMatrix::Multiply(const CMatrix& m0, const CMatrix& m1)
		{
			auto xmm = DirectX::XMMatrixMultiply(m0, m1);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmm);
			return;
		}

		inline void CMatrix::Inverse(const CMatrix& m)
		{
			auto xmmInv = DirectX::XMMatrixInverse(NULL, m);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmInv);
			return;
		}
		inline void CMatrix::Inverse()
		{
			Inverse(*this);
			return;
		}
		inline void CMatrix::Transpose()
		{
			auto xmmTranspose = DirectX::XMMatrixTranspose(*this);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmTranspose);
			return;
		}

		inline void CMatrix::Apply(CVector3& vOut) const
		{
			auto xmv = DirectX::XMVector3Transform(vOut, *this);
			DirectX::XMStoreFloat3(&vOut.m_xmf3Vec, xmv);
			return;
		}
		inline void CMatrix::Apply(CVector4& vOut) const
		{
			auto xmv = DirectX::XMVector4Transform(vOut, *this);
			DirectX::XMStoreFloat4(&vOut.m_xmf4Vec, xmv);
			return;
		}



		inline CMatrix operator*(const CMatrix& m0, const CMatrix& m1)
		{
			CMatrix mRet(m0);
			mRet.Multiply(m1);
			return mRet;
		}



	}
}