#pragma once
namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			class CPath;
			class CNavCell;
			class CNavMesh;
		}
	}
}

namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			class CPathFinding : private nsUtils::SNoncopyable
			{
			private:
				struct SCellWork
				{
					constexpr SCellWork() = default;
					~SCellWork() = default;

					void Init(const CNavCell* cell);

					const CNavCell* cell = nullptr;
					SCellWork* parentCell = nullptr;	// 親のセル。
					float costFromStartCell = 0.0f;
					nsMath::CVector3 pathPoint= nsMath::CVector3::Zero();
					float cost = FLT_MAX;			// 移動コスト
					bool isOpend = false;			// 開かれた？
					bool isClosed = false;			// 閉じられた？
					bool isSmooth = false;			// スムースされる？
				};

			public:
				constexpr CPathFinding() = default;
				~CPathFinding() = default;

				/**
				 * @brief パスの検索処理を実行
				 * @param[out] path 見つかったパスの格納先
				 * @param[in] naviMesh ナビゲーションメッシュ
				 * @param[in] startPos 検索開始座標
				 * @param[in] endPos 検索終了座標
				 * @param[in] enablePhysics 物理判定を有効にするか？
				 * @param[in] agentRadius AIエージェントの半径
				 * @param[in] agentHeight AIエージェントの高さ
				*/
				void Execute(
					CPath& path,
					const CNavMesh& naviMesh,
					const nsMath::CVector3& startPos,
					const nsMath::CVector3& endPos,
					bool enablePhysics = false,
					float agentRadius = 50.0f,
					float agentHeight = 200.0f
				);

			private:

				/**
				 * @brief 次のセルに移動するコストを計算
				 * @param[out] totalCost 全体のコストの格納先
				 * @param[out] costFromStartCell 開始セルからのコストの格納先
				 * @param[in] nextCell 次のセル
				 * @param[in] prevCell 1つ前のセル
				 * @param[in] endCell 最終セル
				*/
				void CalcCost(
					float& totalCost,
					float& costFromStartCell,
					const SCellWork* nextCell,
					const SCellWork* prevCell,
					const CNavCell* endCell
				) const noexcept;

				/**
				 * @brief スムージング
				 * @param[in,out] cellList セルリスト
				 * @param[in] enablePhysics 物理判定を有効にするか？
				 * @param[in] agentRadius AIエージェントの半径
				 * @param[in] agentHeight AIエージェントの高さ
				*/
				void Smoothing(
					std::list<SCellWork*>& cellList,
					bool enablePhysics,
					float agentRadius,
					float agentHeight
				) const noexcept;

				bool IsIntersectRayToCell(
					nsMath::CVector3 rayStartPos,
					nsMath::CVector3 rayEndPos,
					SCellWork* currentCellWork
				) const noexcept;

			private:
				std::vector<SCellWork> m_cellWorkArray = {};

			};

		}
	}
}