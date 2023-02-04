#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			struct SAnimation2DFrameData
			{
				nsMath::CVector2 Position = nsMath::CVector2(0.0f, 0.0f);				//座標
				float RotateDeg = 0.0f;													//回転角度
				nsMath::CVector2 Scale = nsMath::CVector2(1.0f, 1.0f);					//拡大率
				nsMath::CVector4 MulColor = nsMath::CVector4(1.0f, 1.0f, 1.0f, 1.0f);	//乗算カラー
				nsMath::CVector2 Pivot = nsMath::CVector2(0.5f, 0.5f);					//ピボット
			};
		}
	}
}

