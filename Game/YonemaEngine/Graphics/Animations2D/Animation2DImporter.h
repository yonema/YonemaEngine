#pragma once
#include "Animation2DData.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			class CAnimation2DImporter
			{
			public:
				/**
				 * @brief アニメーションを読み込む
				 * @param path アニメーションファイルパス(.tda)
				 * @return 読み込まれたアニメーション
				*/
				SAnimation2DData* Load(const char* path);

			private:
				/**
				 * @brief アニメーションフレームデータをこのエンジンの座標系に変換する
				 * @param frameData アニメーションフレームデータ
				*/
				void ConvertToEngineCoordinate(SAnimation2DFrameData& frameData);
			};
		}
	}
}

