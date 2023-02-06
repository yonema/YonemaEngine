#pragma once

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		struct SLevel2DSpriteData : private nsUtils::SNoncopyable
		{
		public:
			std::string Path = "";						//ファイルパス
			std::string Name = "";						//名前
			nsMath::CVector2 SpriteSize;				//サイズ
			nsMath::CVector2 Position;					//座標
			float RotateDeg = 0.0f;						//回転角度
			nsMath::CVector2 Scale;						//拡大率
			nsMath::CVector4 MulColor;					//乗算カラー
			nsMath::CVector2 Pivot;						//ピボット
			int Priority = 0;							//優先度
			int animNum = 0;							//アニメーション数
			std::vector<std::string> animPathVector;	//アニメーションパスの配列
		};
	}
}