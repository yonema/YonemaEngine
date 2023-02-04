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
				std::string m_animtionName = "";		//�A�j���[�V������
				int m_totalFrame = 0;					//���t���[����
				bool m_loopFlag = false;				//���[�v�t���O
				bool m_isMoveAbsolute = false;			//�ړ����[�h(��Έړ���?)
				std::vector<SAnimation2DFrameData> m_frameDatas;	//�t���[�����Ƃ̒l
				std::unordered_map<std::string,int> m_eventNameMap;	//�A�j���[�V�����C�x���g���Ɣ����t���[�����R�Â���map
			};
		}
	}
}
