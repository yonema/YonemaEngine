#include "Quaternion.h"
#include "Matrix.h"

namespace nsYMEngine
{
	namespace nsMath
	{
		const CQuaternion CQuaternion::m_kIdentity = { 0.0f,  0.0f, 0.0f, 1.0f };

		void CQuaternion::SetRotation(const CMatrix& mat) noexcept
		{
			auto xmv = DirectX::XMQuaternionRotationMatrix(mat);
			DirectX::XMStoreFloat4(&m_xmf4Vec, xmv);
			return;
		}

		void CQuaternion::SetRotation(const CVector3& fromVec, const CVector3& toVec) noexcept
		{
			// fromベクトルとtoベクトルの間の角度を求めて、
			// 回転軸を探して、求めた角度分回転させる。

			auto fromV = fromVec;
			auto toV = toVec;
			fromV.Normalize();
			toV.Normalize();
			CVector3 rotAxis;

			// fromベクトルとtoベクトルの向きを比べる。
			auto t = fromV.Dot(toV);

			if (t > 0.998f)
			{
				// ほぼ同じ向きなので、単位クォータニオンにする。
				*this = Identity();
				return;
			}
			else if (t < -0.998f)
			{
				// ほぼ逆向きなので、適当な軸との外積で、回転軸を求める。
				if (fabsf(toV.x) < 1.0f)
				{
					rotAxis = Cross(CVector3::AxisX(), toV);
				}
				else
				{
					rotAxis = Cross(CVector3::AxisY(), toV);
				}
			}
			else
			{
				rotAxis = Cross(fromV, toV);
			}

			rotAxis.Normalize();
			float radAngle = acosf(t);
			SetRotation(rotAxis, radAngle);
			return;
		}


	}
}