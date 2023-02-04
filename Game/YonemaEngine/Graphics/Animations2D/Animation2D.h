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
				 * @brief アニメーションの初期化
				 * @param fileName アニメーションのファイルパス
				*/
				void Init(const char* fileName);

				/**
				 * @brief アニメーションを再生開始する
				*/
				void PlayAnimation()
				{
					m_isPlaying = true;
				}

				/**
				 * @brief アニメーションを更新する
				*/
				void Update();

				/**
				 * @brief フレームデータを取得する
				 * @return 
				*/
				const SAnimation2DFrameData& GetFrameData();

				/**
				 * @brief 再生のキャンセル(座標は元の位置に戻る)
				*/
				void Cancel();

				/**
				 * @brief 強制的にアニメーションの最終フレームの位置で停止する
				*/
				void ForceEnd();

				/**
				 * @brief アニメーションイベントが発生した際に呼ばれる処理を登録する
				 * @param name 登録するアニメーションイベント名
				 * @param hookFunc 登録するイベント
				*/
				void RegisterEvent(std::string eventName, std::function<void(const SAnimation2DFrameData&)> eventFunc);

				/**
				* @brief アニメーションが再生中かどうかを調べる
				* @return アニメーションを再生中?
				*/
				bool IsPlaying()
				{
					return m_isPlaying;
				}

				/**
				 * @brief アニメーション再生前の座標を保存しておく
				 * @param position アニメーション再生前の座標
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

				//アニメーション自身のデータ
				SAnimation2DData* m_animationData;

				//再生に関するデータ
				nsMath::CVector2 m_defaultPosition = nsMath::CVector2(0.0f, 0.0f);	//再生開始時の座標
				nsMath::CVector2 m_currentPosition = nsMath::CVector2(0.0f, 0.0f);	//再生されている現在の座標
				SAnimation2DFrameData m_playingFrameData;			//再生中のアニメーションのフレームデータ
				bool m_isPlaying = false;							//再生中?
				int m_currentFrame = 0;								//再生中のフレーム数
				std::unordered_map<int, std::function<void(const SAnimation2DFrameData&)>> m_events;	//アニメーションイベント
			};
		}
	}
}