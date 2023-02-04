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
				 * @brief �A�j���[�V������ǂݍ���
				 * @param path �A�j���[�V�����t�@�C���p�X(.tda)
				 * @return �ǂݍ��܂ꂽ�A�j���[�V����
				*/
				SAnimation2DData* Load(const char* path);

			private:
				/**
				 * @brief �A�j���[�V�����t���[���f�[�^�����̃G���W���̍��W�n�ɕϊ�����
				 * @param frameData �A�j���[�V�����t���[���f�[�^
				*/
				void ConvertToEngineCoordinate(SAnimation2DFrameData& frameData);
			};
		}
	}
}

