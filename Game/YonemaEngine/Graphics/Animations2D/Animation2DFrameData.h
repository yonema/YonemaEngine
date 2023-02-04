#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			struct SAnimation2DFrameData
			{
				nsMath::CVector2 Position = nsMath::CVector2(0.0f, 0.0f);				//���W
				float RotateDeg = 0.0f;													//��]�p�x
				nsMath::CVector2 Scale = nsMath::CVector2(1.0f, 1.0f);					//�g�嗦
				nsMath::CVector4 MulColor = nsMath::CVector4(1.0f, 1.0f, 1.0f, 1.0f);	//��Z�J���[
				nsMath::CVector2 Pivot = nsMath::CVector2(0.5f, 0.5f);					//�s�{�b�g
			};
		}
	}
}

