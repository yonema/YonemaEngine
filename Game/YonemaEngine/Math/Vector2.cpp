#include "Vector2.h"
namespace nsYMEngine
{
	namespace nsMath
	{
		const CVector2 CVector2::m_kZero = { 0.0f,0.0f };
		const CVector2 CVector2::m_kOne = { 1.0f,1.0f };
		const CVector2 CVector2::m_kRight = { 1.0f,0.0f };
		const CVector2 CVector2::m_kLeft = { -1.0f,0.0f };
		const CVector2 CVector2::m_kUp = { 0.0f,1.0f };
		const CVector2 CVector2::m_kDown = { 0.0f,-1.0f };
		const CVector2 CVector2::m_kAxisX = { 1.0f,0.0f };
		const CVector2 CVector2::m_kAxisY = { 0.0f,1.0f };
		const CVector2 CVector2::m_kCenter = { 0.5f,0.5f };

	}
}