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
			// from�x�N�g����to�x�N�g���̊Ԃ̊p�x�����߂āA
			// ��]����T���āA���߂��p�x����]������B

			auto fromV = fromVec;
			auto toV = toVec;
			fromV.Normalize();
			toV.Normalize();
			CVector3 rotAxis;

			// from�x�N�g����to�x�N�g���̌������ׂ�B
			auto t = fromV.Dot(toV);

			if (t > 0.998f)
			{
				// �قړ��������Ȃ̂ŁA�P�ʃN�H�[�^�j�I���ɂ���B
				*this = Identity();
				return;
			}
			else if (t < -0.998f)
			{
				// �قڋt�����Ȃ̂ŁA�K���Ȏ��Ƃ̊O�ςŁA��]�������߂�B
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