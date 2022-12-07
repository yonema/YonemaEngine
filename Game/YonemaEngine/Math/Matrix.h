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
			constexpr explicit CMatrix() noexcept;
			constexpr CMatrix(
				float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33
			) noexcept;
			constexpr CMatrix(const CMatrix& m) noexcept;
			constexpr explicit CMatrix(const DirectX::XMFLOAT4X4& m) noexcept;

			inline ~CMatrix() = default;

		public:
			inline operator DirectX::XMMATRIX() const noexcept;

			inline CMatrix& operator=(const CMatrix& m) noexcept;
			inline CMatrix& operator*=(const CMatrix& m) noexcept;

		public:
			static inline const CMatrix& Identity() noexcept;
			static inline const CMatrix& Zero() noexcept;
			static inline CMatrix Translation(const CVector3& offset) noexcept;
			static inline CMatrix Translation(float x, float y, float z) noexcept;
			static inline CMatrix RotationAxis(const CVector3& axis, float radAngle) noexcept;
			static inline CMatrix RotationX(float radAngle) noexcept;
			static inline CMatrix RotationY(float radAngle) noexcept;
			static inline CMatrix RotationZ(float radAngle) noexcept;
			static inline CMatrix RotationFromQuaternion(nsMath::CQuaternion& q) noexcept;
			static inline CMatrix ProjectionMatrix(
				float fovAngleY, float aspectRatio, float nearClip, float farClip) noexcept;
			static inline CMatrix OrthoProjectionMatrix(
				float viewWidth, float viewHeight, float nearClip, float farClip) noexcept;
			static inline CMatrix ViewMatrix(
				const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir) noexcept;

			inline void MakeTranslation(const CVector3& offset) noexcept;
			inline void MakeTranslation(float x, float y, float z) noexcept;
			inline void MakeRotationX(float radAngle) noexcept;
			inline void MakeRotationY(float radAngle) noexcept;
			inline void MakeRotationZ(float radAngle) noexcept;
			inline void MakeRotationAxis(const CVector3& axis, float radAngle) noexcept;
			inline void MakeRotationFromQuaternion(const CQuaternion& q) noexcept;
			inline void MakeScaling(const CVector3& scale) noexcept;
			inline void MakeScaling(float x, float y, float z) noexcept;

			inline void MakeProjectionMatrix(
				float fovAngleY, float aspectRatio, float nearClip, float farClip) noexcept;
			inline void MakeOrthoProjectionMatrix(
				float viewWidth, float viewHeight, float nearClip, float farClip) noexcept;
			inline void MakeViewMatrix(
				const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir) noexcept;

			inline void Multiply(const CMatrix& m) noexcept;
			inline void Multiply(const CMatrix& m0, const CMatrix& m1) noexcept;

			inline void Inverse(const CMatrix& m) noexcept;
			inline void Inverse() noexcept;
			inline void Transpose() noexcept;

			inline void Apply(CVector3& vOut) const noexcept;
			inline void Apply(CVector4& vOut) const noexcept;




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


		static inline CMatrix operator*(const CMatrix& m0, const CMatrix& m1) noexcept;



		static inline void CalcTranslateFromMatrix(
			CVector3& transOut, const CMatrix& srcMat) noexcept;
		void CalcRotationFromMatrix(
			CQuaternion& rotOut, const CMatrix& srcMat) noexcept;
		static inline void CalcScaleFromMatrix(CVector3& scaleOut, const CMatrix& srcMat) noexcept;

		void CalcTRSFromMatrix(
			CVector3& transOut, CQuaternion& rotOut, CVector3& scaleOut, const CMatrix& srcMat
		) noexcept;

		//////////////// 関数定義 ////////////////


		constexpr CMatrix::CMatrix() noexcept
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
		) noexcept
			:m_xmf4x4Mat(
				m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33
			)
		{
			return;
		}
		constexpr CMatrix::CMatrix(const CMatrix& m) noexcept
			:m_xmf4x4Mat(m.m_xmf4x4Mat)
		{
			return;
		}
		constexpr CMatrix::CMatrix(const DirectX::XMFLOAT4X4& m) noexcept
			:m_xmf4x4Mat(m)
		{
			return;
		}


		inline CMatrix::operator DirectX::XMMATRIX() const noexcept
		{
			return DirectX::XMLoadFloat4x4(&m_xmf4x4Mat);
		}

		inline CMatrix& CMatrix::operator=(const CMatrix& m) noexcept
		{
			m_xmf4x4Mat = m.m_xmf4x4Mat;
			return *this;
		}
		inline CMatrix& CMatrix::operator*=(const CMatrix& m) noexcept
		{
			Multiply(m);
			return *this;
		}


		inline const CMatrix& CMatrix::Identity() noexcept
		{
			return m_kIdentity;
		}
		inline const CMatrix& CMatrix::Zero() noexcept
		{
			return m_kZero;
		}
		inline CMatrix CMatrix::Translation(const CVector3& offset) noexcept
		{
			CMatrix mat;
			mat.MakeTranslation(offset);
			return mat;
		}
		inline CMatrix CMatrix::Translation(float x, float y, float z) noexcept
		{
			CMatrix mat;
			mat.MakeTranslation(x, y, z);
			return mat;
		}
		inline CMatrix CMatrix::RotationAxis(const CVector3& axis, float radAngle) noexcept
		{
			CMatrix mat;
			mat.MakeRotationAxis(axis, radAngle);
			return mat;
		}
		inline CMatrix CMatrix::RotationX(float radAngle) noexcept
		{
			CMatrix mat;
			mat.MakeRotationX(radAngle);
			return mat;
		}
		inline CMatrix CMatrix::RotationY(float radAngle) noexcept
		{
			CMatrix mat;
			mat.MakeRotationY(radAngle);
			return mat;
		}
		inline CMatrix CMatrix::RotationZ(float radAngle) noexcept
		{
			CMatrix mat;
			mat.MakeRotationZ(radAngle);
			return mat;
		}
		inline CMatrix CMatrix::RotationFromQuaternion(nsMath::CQuaternion& q) noexcept
		{
			CMatrix mat;
			mat.MakeRotationFromQuaternion(q);
			return mat;
		}
		inline CMatrix CMatrix::ProjectionMatrix(
			float fovAngleY, float aspectRatio, float nearClip, float farClip) noexcept
		{
			CMatrix mat;
			mat.MakeProjectionMatrix(fovAngleY, aspectRatio, nearClip, farClip);
			return mat;
		}
		inline CMatrix CMatrix::OrthoProjectionMatrix(
			float viewWidth, float viewHeight, float nearClip, float farClip) noexcept
		{
			CMatrix mat;
			mat.MakeOrthoProjectionMatrix(viewWidth, viewHeight, nearClip, farClip);
			return mat;
		}
		inline CMatrix CMatrix::ViewMatrix(
			const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir) noexcept
		{
			CMatrix mat;
			mat.MakeViewMatrix(eyePos, targetPos, upDir);
			return mat;
		}

		inline void CMatrix::MakeTranslation(const CVector3& offset) noexcept
		{
			auto xmmTrans = DirectX::XMMatrixTranslationFromVector(offset);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmTrans);
			return;
		}
		inline void CMatrix::MakeTranslation(float x, float y, float z) noexcept
		{
			MakeTranslation(CVector3(x, y, z));
			return;
		}
		inline void CMatrix::MakeRotationX(float radAngle) noexcept
		{
			auto xmmRot = DirectX::XMMatrixRotationX(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationY(float radAngle) noexcept
		{
			auto xmmRot = DirectX::XMMatrixRotationY(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationZ(float radAngle) noexcept
		{
			auto xmmRot = DirectX::XMMatrixRotationZ(radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationAxis(const CVector3& axis, float radAngle) noexcept
		{
			auto xmmRot = DirectX::XMMatrixRotationAxis(axis, radAngle);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeRotationFromQuaternion(const CQuaternion& q) noexcept
		{
			auto xmmRot = DirectX::XMMatrixRotationQuaternion(q);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmRot);
			return;
		}
		inline void CMatrix::MakeScaling(const CVector3& scale) noexcept
		{
			auto xmmScale = DirectX::XMMatrixScalingFromVector(scale);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmScale);
			return;
		}
		inline void CMatrix::MakeScaling(float x, float y, float z) noexcept
		{
			MakeScaling(CVector3(x, y, z));
			return;
		}


		inline void CMatrix::MakeProjectionMatrix(
			float fovAngleY, float aspectRatio, float nearClip, float farClip) noexcept
		{
			auto xmmProj =
				DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearClip, farClip);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmProj);
			return;
		}
		inline void CMatrix::MakeOrthoProjectionMatrix(
			float viewWidth, float viewHeight, float nearClip, float farClip) noexcept
		{
			auto xmmProj =
				DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearClip, farClip);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmProj);
			return;
		}
		inline void CMatrix::MakeViewMatrix(
			const CVector3& eyePos, const CVector3& targetPos, const CVector3& upDir) noexcept
		{
			auto xmmView =
				DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmView);
			return;
		}

		inline void CMatrix::Multiply(const CMatrix& m) noexcept
		{
			auto xmm = DirectX::XMMatrixMultiply(*this, m);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmm);
			return;
		}
		inline void CMatrix::Multiply(const CMatrix& m0, const CMatrix& m1) noexcept
		{
			auto xmm = DirectX::XMMatrixMultiply(m0, m1);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmm);
			return;
		}

		inline void CMatrix::Inverse(const CMatrix& m) noexcept
		{
			auto xmmInv = DirectX::XMMatrixInverse(NULL, m);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmInv);
			return;
		}
		inline void CMatrix::Inverse() noexcept
		{
			Inverse(*this);
			return;
		}
		inline void CMatrix::Transpose() noexcept
		{
			auto xmmTranspose = DirectX::XMMatrixTranspose(*this);
			DirectX::XMStoreFloat4x4(&m_xmf4x4Mat, xmmTranspose);
			return;
		}

		inline void CMatrix::Apply(CVector3& vOut) const noexcept
		{
			auto xmv = DirectX::XMVector3Transform(vOut, *this);
			DirectX::XMStoreFloat3(&vOut.m_xmf3Vec, xmv);
			return;
		}
		inline void CMatrix::Apply(CVector4& vOut) const noexcept
		{
			auto xmv = DirectX::XMVector4Transform(vOut, *this);
			DirectX::XMStoreFloat4(&vOut.m_xmf4Vec, xmv);
			return;
		}



		inline CMatrix operator*(const CMatrix& m0, const CMatrix& m1) noexcept
		{
			CMatrix mRet(m0);
			mRet.Multiply(m1);
			return mRet;
		}

		inline void CalcTranslateFromMatrix(
			CVector3& transOut, const CMatrix& srcMat) noexcept
		{
			transOut = { srcMat.m_fMat[3][0], srcMat.m_fMat[3][1], srcMat.m_fMat[3][2] };
			return;
		}

		inline void CalcScaleFromMatrix(CVector3& scaleOut, const CMatrix& srcMat) noexcept
		{
			scaleOut.x = srcMat.m_vec4Mat[0].Length();
			scaleOut.y = srcMat.m_vec4Mat[1].Length();
			scaleOut.z = srcMat.m_vec4Mat[2].Length();
			return;
		}


	}
}