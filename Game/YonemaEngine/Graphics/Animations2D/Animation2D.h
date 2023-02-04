#pragma once
#include "Animation2DData.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			class CAnimation2D
			{
			public:
				/**
				 * @brief �A�j���[�V�����̏�����
				 * @param fileName �A�j���[�V�����̃t�@�C���p�X
				*/
				void Init(const char* fileName);

				/**
				 * @brief �A�j���[�V�������Đ��J�n����
				*/
				void PlayAnimation()
				{
					m_isPlaying = true;
				}

				/**
				 * @brief �A�j���[�V�������X�V����
				*/
				void Update();

				/**
				 * @brief �t���[���f�[�^���擾����
				 * @return 
				*/
				const SAnimation2DFrameData& GetFrameData();

				/**
				 * @brief �Đ��̃L�����Z��(���W�͌��̈ʒu�ɖ߂�)
				*/
				void Cancel();

				/**
				 * @brief �����I�ɃA�j���[�V�����̍ŏI�t���[���̈ʒu�Œ�~����
				*/
				void ForceEnd();

				/**
				 * @brief �A�j���[�V�����C�x���g�����������ۂɌĂ΂�鏈����o�^����
				 * @param name �o�^����A�j���[�V�����C�x���g��
				 * @param hookFunc �o�^����C�x���g
				*/
				void RegisterEvent(std::string eventName, std::function<void(const SAnimation2DFrameData&)> eventFunc);

				/**
				* @brief �A�j���[�V�������Đ������ǂ����𒲂ׂ�
				* @return �A�j���[�V�������Đ���?
				*/
				bool IsPlaying()
				{
					return m_isPlaying;
				}

				/**
				 * @brief �A�j���[�V�����Đ��O�̍��W��ۑ����Ă���
				 * @param position �A�j���[�V�����Đ��O�̍��W
				*/
				void SetDefaultPosition(const nsMath::CVector2& position)
				{
					m_defaultPosition = position;
					m_currentPosition = position;
					m_playingFrameData.Position = position;
				}

				void ResetFrame()
				{
					m_currentFrame = 0;
					m_isPlaying = false;
				}

				//�A�j���[�V�������g�̃f�[�^
				SAnimation2DData* m_animationData;

				//�Đ��Ɋւ���f�[�^
				nsMath::CVector2 m_defaultPosition = nsMath::CVector2(0.0f, 0.0f);	//�Đ��J�n���̍��W
				nsMath::CVector2 m_currentPosition = nsMath::CVector2(0.0f, 0.0f);	//�Đ�����Ă��錻�݂̍��W
				SAnimation2DFrameData m_playingFrameData;			//�Đ����̃A�j���[�V�����̃t���[���f�[�^
				bool m_isPlaying = false;							//�Đ���?
				int m_currentFrame = 0;								//�Đ����̃t���[����
				std::unordered_map<int, std::function<void(const SAnimation2DFrameData&)>> m_events;	//�A�j���[�V�����C�x���g
			};
		}
	}
}