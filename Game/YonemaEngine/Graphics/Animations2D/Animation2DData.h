#pragma once
#include "Animation2DFrameData.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			struct SAnimation2DData : private nsUtils::SNoncopyable
			{
				std::string m_animtionName = "";		//アニメーション名
				int m_totalFrame = 0;					//総フレーム数
				bool m_loopFlag = false;				//ループフラグ
				bool m_isMoveAbsolute = false;			//移動モード(絶対移動か?)
				std::vector<SAnimation2DFrameData> m_frameDatas;	//フレームごとの値
				std::unordered_map<std::string,int> m_eventNameMap;	//アニメーションイベント名と発生フレームが紐づいたmap
			};
		}
	}
}
