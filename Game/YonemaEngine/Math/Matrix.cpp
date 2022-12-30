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
			// âÒì]ê¨ï™à»äOÇèúãé
			auto tmpMat = srcMat;
			tmpMat.m_vec4Mat[0].Normalize();
			tmpMat.m_vec4Mat[1].Normalize();
			tmpMat.m_vec4Mat[2].Normalize();
			tmpMat.m_vec4Mat[3] = { 0.0f,0.0f,0.0f,1.0f };
			rotOut.SetRotation(tmpMat);

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