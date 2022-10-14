#include "Vector3.h"

namespace nsYMEngine
{
	namespace nsMath
	{
		const CVector3 CVector3::m_kZero = { 0.0f,0.0f,0.0f };
		const CVector3 CVector3::m_kOne = { 1.0f,1.0f,1.0f };
		const CVector3 CVector3::m_kRight = { 1.0f,0.0f,0.0f };
		const CVector3 CVector3::m_kLeft = { -1.0f,0.0f,0.0f };
		const CVector3 CVector3::m_kUp = { 0.0f,1.0f,0.0f };
		const CVector3 CVector3::m_kDown = { 0.0f,-1.0f,0.0f };
		const CVector3 CVector3::m_kFront = { 0.0f,0.0f,1.0f };
		const CVector3 CVector3::m_kBack = { 0.0f,0.0f,-1.0f };
		const CVector3 CVector3::m_kAxisX = { 1.0f,0.0f,0.0f };
		const CVector3 CVector3::m_kAxisY = { 0.0f,1.0f,0.0f };
		const CVector3 CVector3::m_kAxisZ = { 0.0f,0.0f,1.0f };


	}
}