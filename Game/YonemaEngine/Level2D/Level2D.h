#pragma once
#include "Level2DSpriteData.h"

namespace nsYMEngine
{
	namespace nsLevel2D
	{
		class CLevel2D : private nsUtils::SNoncopyable
		{
		public:
			/**
			 * @brief 2Dレベルデータをロードする
			 * @param path 2Dレベルデータのパス
			 * @param hookFunc hook関数
			*/
			void Load(const char* path, std::function<bool(const SLevel2DSpriteData&)> hookFunc = nullptr);

			/**
			 * @brief レベルに存在するスプライトを削除する
			*/
			void Clear()
			{
				for (nsGraphics::nsRenderers::CAnimatedSpriteRenderer* sprite : m_spritesList)
				{
					DeleteGO(sprite);
				}

				m_spritesList.resize(0);
			}

			/**
			 * @brief レベルに存在するスプライトのアニメーションを再生する
			 * @param animationName アニメーションの名前
			*/
			void PlayAnimation(std::string animationName)
			{
				for (nsGraphics::nsRenderers::CAnimatedSpriteRenderer* sprite : m_spritesList)
				{
					sprite->PlayAnimation(animationName);
				}
			}

			~CLevel2D()
			{
				for (nsGraphics::nsRenderers::CAnimatedSpriteRenderer* sprite : m_spritesList)
				{
					DeleteGO(sprite);
				}
			}
		private:
			/**
			 * @brief ツールで生成されたスプライトデータをエンジンの座標系に変換する
			 * @param spriteData ツールで生成されたスプライトのデータ
			*/
			void ConvertToEngineCoordinate(SLevel2DSpriteData& spriteData);

			/**
			 * @brief スプライトのデータを元にスプライトを作成する
			 * @param spriteData 
			 * @return 作成したスプライトレンダラー
			*/
			nsGraphics::nsRenderers::CAnimatedSpriteRenderer* CreateNewSpriteFromData(const SLevel2DSpriteData& spriteData);

			//スプライトのリスト
			std::list<nsGraphics::nsRenderers::CAnimatedSpriteRenderer*> m_spritesList;
		};
	}
}

