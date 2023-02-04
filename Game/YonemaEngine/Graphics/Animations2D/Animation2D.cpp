#include "YonemaEnginePreCompile.h"
#include "Animation2D.h"
#include "Animation2DImporter.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations2D
		{
			void CAnimation2D::Init(const char* fileName)
			{
				CAnimation2DImporter importer;
				m_animationData = importer.Load(fileName);
			}

			void CAnimation2D::Update()
			{
				if (m_isPlaying == false)
				{
					return;
				}

				m_playingFrameData = m_animationData->m_frameDatas[m_currentFrame];

				if (m_animationData->m_isMoveAbsolute == false)
				{
					m_currentPosition += m_playingFrameData.Position;
					m_playingFrameData.Position = m_currentPosition;
				}

				m_currentFrame++;

				//アニメーションの最後に達した場合
				if (m_currentFrame == m_animationData->m_totalFrame)
				{
					m_currentFrame = 0;
					if (m_animationData->m_loopFlag == false)
					{
						m_isPlaying = false;
					}
				}

				//イベントのチェック
				if (m_events.count(m_currentFrame + 1) == 1)
				{
					m_events.at(m_currentFrame + 1)(m_playingFrameData);
				}

				return;
			}

			const SAnimation2DFrameData& CAnimation2D::GetFrameData()
			{
				return m_playingFrameData;
			}

			void CAnimation2D::ForceEnd()
			{
				if (m_isPlaying == false)
				{
					return;
				}

				//移動モードで分岐
				if (m_animationData->m_isMoveAbsolute == true)
				{
					const auto& frameData = m_animationData->m_frameDatas[static_cast<size_t>(m_animationData->m_totalFrame) - 1];
					m_currentFrame = m_animationData->m_totalFrame - 1;

					m_playingFrameData = frameData;
				}
				else
				{
					auto frameData = m_animationData->m_frameDatas[static_cast<size_t>(m_animationData->m_totalFrame) - 1];
					CVector2 moveAmount = CVector2(0.0f, 0.0f);

					for (int i = m_currentFrame;i < m_animationData->m_totalFrame;i++)
					{
						moveAmount += m_animationData->m_frameDatas[i].Position;
					}

					m_currentPosition += moveAmount;
					frameData.Position = m_currentPosition;

					m_currentFrame = 0;
					m_playingFrameData = frameData;
				}

				m_isPlaying = false;
			}

			void CAnimation2D::Cancel()
			{
				if (m_isPlaying == false)
				{
					return;
				}

				m_currentPosition = m_defaultPosition;
				m_playingFrameData = m_animationData->m_frameDatas[0];
				m_currentFrame = 0;

				m_isPlaying = false;
			}

			void CAnimation2D::RegisterEvent(std::string eventName, std::function<void(const SAnimation2DFrameData&)> eventFunc)
			{
				int count = m_animationData->m_eventNameMap.count(eventName);

				if (count != 1)
				{
					return;
				}

				int eventFrame = m_animationData->m_eventNameMap.at(eventName);

				m_events.insert({ eventFrame, eventFunc });
			}
		}
	}
}