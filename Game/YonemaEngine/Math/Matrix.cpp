#include "Matrix.h"
namespace nsYMEngine
{
	namespace nsMath
	{
		const CMatrix CMatrix::m_kIdentity =
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		const CMatrix CMatrix::m_kZero =
		{
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f
		};

		void CalcRotationFromMatrix(CQuaternion& rotOut, const CMatrix& srcMat) noexcept
		{
			rotOut.SetRotation(srcMat);

			return;
		}

		void CalcTRSFromMatrix(
			CVector3& transOut, CQuaternion& rotOut, CVector3& scaleOut, const CMatrix& srcMat
		) noexcept
		{
			CalcTranslateFromMatrix(transOut, srcMat);
			CalcRotationFromMatrix(rotOut, srcMat);
			CalcScaleFromMatrix(scaleOut, srcMat);

			return;
		}


	}
}