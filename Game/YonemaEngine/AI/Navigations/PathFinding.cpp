#include "PathFinding.h"
#include "Path.h"
#include "NavMesh.h"
#include "../Physics/PhysicsObject.h"
#include "../Physics/PhysicsEngine.h"

namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{
			void CPathFinding::SCellWork::Init(const CNavCell* cell)
			{
				this->cell = cell;
				pathPoint = cell->GetCenterPosition();
				parentCell = nullptr;
				costFromStartCell = 0.0f;
				cost = FLT_MAX;
				isOpend = false;
				isClosed = false;
				isSmooth = false;

				return;
			}


			void CPathFinding::Execute(
				CPath& path,
				const CNavMesh& naviMesh,
				const nsMath::CVector3& startPos,
				const nsMath::CVector3& endPos,
				bool enablePhysics,
				float agentRadius,
				float agentHeight
			)
			{
				// パスをクリア。
				path.Clear();
				// 開始位置に最も近いセルを検索。
				const CNavCell& startCell = naviMesh.FindNearestNavCell(startPos);
				// 終了位置に最も近いセルを検索。
				const CNavCell& endCell = naviMesh.FindNearestNavCell(endPos);
				// A*を解く時に、セルにいろいろな情報を埋め込むので、情報を埋め込む作業領域を確保。
				m_cellWorkArray.resize(naviMesh.GetNumCell());

				// 作業領域を初期化する。
				for (int cellNo = 0; cellNo < m_cellWorkArray.size(); cellNo++)
				{
					m_cellWorkArray[cellNo].Init(&naviMesh.GetCell(cellNo));
				}
				// 最後にパスを作るときの座標を設定する。
				// 開始セルは開始座標、終了セルは終了座標。
				// それ以外のセルは、セルの中心座標がCellWork::Init()関数で設定されている。
				m_cellWorkArray[startCell.GetCellNo()].pathPoint = startPos;
				m_cellWorkArray[endCell.GetCellNo()].pathPoint = endPos;

				using namespace std;
				list<SCellWork*> openList;
				// 開始セルの作業領域を取得。
				SCellWork* crtCell = &m_cellWorkArray.at(startCell.GetCellNo());
				// 開始セルの移動コストを計算する。
				CalcCost(crtCell->cost, crtCell->costFromStartCell, crtCell, nullptr, &endCell);
				// 開始セルをオープンリストに積む。
				openList.emplace_back(crtCell);
				// セルをオープンリストに積んだ印をつける。
				crtCell->isOpend = true;

				// ここからA*アルゴリズムを解いていくｊ。
				bool isSuccess = false;
				while (!openList.empty())
				{
					// オープンリストから最もコストの安いセルを取得。
					crtCell = nullptr;
					float lowestCost = FLT_MAX;
					list<SCellWork*>::iterator lowestCellIt;
					for (auto it = openList.begin(); it != openList.end(); it++)
					{
						if (lowestCost > (*it)->cost) 
						{
							// このセルの方が安い。
							lowestCost = (*it)->cost;
							crtCell = (*it);
							lowestCellIt = it;
						}
					}
					// 取り出したセルをオープンリストから削除。
					openList.erase(lowestCellIt);

					if (crtCell->cell == &endCell)
					{
						// オープンリストから取り出したセルが終了せるなら経路探索成功。
						isSuccess = true;
						break;
					}

					// 開いたセルの隣接セルをオープンリストに積んでいく。
					for (int linkCellNo = 0; linkCellNo < 3; linkCellNo++)
					{
						CNavCell* linkCell = crtCell->cell->GetLinkCell(linkCellNo);
						if (linkCell != nullptr)
						{
							// 隣接セルがあったので、コストを計算してオープンリストに積む。
							SCellWork* linkCellWork = &m_cellWorkArray.at(linkCell->GetCellNo());

							//隣接セルに移動するコストを計算
							float newCost;
							float newCostFromStartCell;
							CalcCost(newCost, newCostFromStartCell, linkCellWork, crtCell, &endCell);

							if (linkCellWork->isClosed == false)
							{
								//隣接セルの調査は終わっていない。

								if (linkCellWork->cost > newCost)
								{
									// こちらのコストの方が安いので改善する。
									linkCellWork->costFromStartCell = newCostFromStartCell;
									linkCellWork->cost = newCost;
									// 親を設定。
									linkCellWork->parentCell = crtCell;
								}
								if (linkCellWork->isOpend == false)
								{
									// まだオープンリストに積む。
									linkCellWork->isOpend = true;
									openList.emplace_back(linkCellWork);
								}
							}

						}
					}
					// このセルの調査は終了なので、クローズの印をつける。
					crtCell->isClosed = true;
				}

				if (isSuccess)
				{
					// パスを構築。
					list<SCellWork*> cellList;
					crtCell = &m_cellWorkArray.at(endCell.GetCellNo());
					while (true)
					{
						if (crtCell->cell == &startCell)
						{
							// 開始セルに到達した。
							// 最後に開始セルを積む。
							cellList.emplace_back(crtCell);
							break;
						}
						cellList.emplace_back(crtCell);
						crtCell = crtCell->parentCell;
					}
					// 反転させる。
					std::reverse(cellList.begin(), cellList.end());
					//
					Smoothing(cellList, enablePhysics, agentRadius, agentHeight);

					// ポイントをパスに積んでいく
					path.ReservePoint(static_cast<unsigned int>(cellList.size()));
					for (auto it = cellList.begin(); it != cellList.end(); it++)
					{
						path.AddPoint((*it)->pathPoint);
					}

					// パスを構築する。
					path.Build();

				}

				return;
			}

			void CPathFinding::CalcCost(
				float& totalCost,
				float& costFromStartCell,
				const SCellWork* nextCell,
				const SCellWork* prevCell,
				const CNavCell* endCell
			) const noexcept
			{
				if (prevCell == nullptr)
				{
					// 直前のセルがない。
					costFromStartCell = 0.0f;
				}
				else
				{
					// 直前のセルがある。
					nsMath::CVector3 t = 
						nextCell->cell->GetCenterPosition() - prevCell->cell->GetCenterPosition();
					costFromStartCell = t.Length() + prevCell->costFromStartCell;
				}

				float distToEndPos = 
					(nextCell->cell->GetCenterPosition() - endCell->GetCenterPosition()).Length();
				totalCost = costFromStartCell + distToEndPos;

				return;
			}


			void CPathFinding::Smoothing(
				std::list<SCellWork*>& cellList,
				bool enablePhysics,
				float agentRadius,
				float agentHeight
			) const noexcept
			{
				// パスの可視判定を行って、不要なセルを除外していく。
				if (cellList.size() < 3)
				{
					// セルの数が3以下ならスムージングする必要なし。
					return;
				}

				int skipCellCount = static_cast<int>(cellList.size()) - 1;
				while (skipCellCount > 2)
				{
					// セルの数が３以上なら、パスの可視判定を行って、不要なセルを除外していく。
					// レイの始点となるセル。
					auto rayStartCellIt = cellList.begin();
					// レイの終点のセル。
					auto rayEndCellIt = rayStartCellIt;
					for (int i = 0; i < skipCellCount; i++)
					{
						rayEndCellIt++;
					}
					bool isEnd = false;
					while (isEnd == false)
					{
						// レイの終端のセルまでレイテストをXZ平面で行う。
						nsMath::CVector3 rayStartPos = (*rayStartCellIt)->cell->GetCenterPosition();
						nsMath::CVector3 rayEndPos = (*rayEndCellIt)->cell->GetCenterPosition();
						rayStartPos.y = 0.0f;
						rayEndPos.y = 0.0f;
						bool isVisible = true;
						auto cellIt = rayStartCellIt;
						do
						{
							cellIt++;
							isVisible = IsIntersectRayToCell(rayStartPos, rayEndPos, *cellIt);
							if (isVisible == false)
							{
								cellIt = rayEndCellIt;
								break;
							}
						} while (cellIt != rayEndCellIt);

						if (isVisible)
						{
							// 可視
							// 始点から終点が見えるということは、始点～終点までの間のセルは削除できるのでスムースマークを付ける。
							auto cellIt = rayStartCellIt;
							cellIt++;
							while (cellIt != rayEndCellIt)
							{
								(*cellIt)->isSmooth = true;
								cellIt++;
							}
						}
						// 次。
						rayStartCellIt = rayEndCellIt;
						rayEndCellIt = rayStartCellIt;
						for (int i = 0; i < skipCellCount; i++)
						{
							rayEndCellIt++;
							if (rayEndCellIt == cellList.end())
							{
								isEnd = true;
								break;
							}
						}
					}
					skipCellCount /= 2;
				}

				// スムースフラグが立っているセルを除去していく
				cellList.remove_if(
					[&](SCellWork* cell)
					{
						if (cell->isSmooth)
						{
							return true;
						}
						return false;
					}
				);

				// 物理オブジェクトとの衝突判定を行い、削除できるセルは削除する。
#if 0
				if (enablePhysics && cellList.size() > 2)
				{
					auto collider = physx::PxCapsuleGeometry(agentRadius, agentHeight);

					auto rayStartCellIt = cellList.begin();
					auto rayPrevCellit = rayStartCellIt;
					rayPrevCellit++;
					auto rayEndCellIt = rayPrevCellit;
					rayEndCellIt++;
					while (rayEndCellIt != cellList.end())
					{
						nsMath::CVector3 rayStartPos = (*rayStartCellIt)->pathPoint;
						nsMath::CVector3 rayEndPos = (*rayEndCellIt)->pathPoint;
						float offset = agentHeight * 0.5f + agentRadius + agentHeight * 0.1f;
						offset = 0.0f;
						rayStartPos.y += offset;
						rayEndPos.y += offset;
						auto moveDir = rayEndPos - rayStartPos;
						float moveDist = moveDir.Length();
						moveDir.Normalize();

						physx::PxSweepHit hitBuffers[32];
						physx::PxSweepBuffer hitInfo(hitBuffers, 32);

						const auto& secne = nsPhysics::CPhysicsEngine::GetInstance()->GetScene();
						physx::PxTransform pose(rayStartPos.x, rayStartPos.y, rayStartPos.z);
						static const physx::PxHitFlags hitFlags =
							physx::PxHitFlag::ePOSITION |
							physx::PxHitFlag::eNORMAL |
							physx::PxHitFlag::ePRECISE_SWEEP |
							physx::PxHitFlag::eMESH_ANY;
						static const physx::PxQueryFilterData filter(
							physx::PxQueryFlag::eANY_HIT |
							physx::PxQueryFlag::eDYNAMIC |
							physx::PxQueryFlag::eSTATIC
						);
						bool isHit = secne->sweep(
							collider,
							pose,
							{ moveDir.x, moveDir.y, moveDir.z},
							moveDist,
							hitInfo,
							hitFlags,
							filter
							);
						if (isHit)
						{
							// 何かのオブジェクトとぶつかった。
							// 始点から終点の間のセルは削除できない。
							// 次のブロックを見ていく。
							rayStartCellIt = rayEndCellIt;
							rayPrevCellit = rayEndCellIt;
							rayPrevCellit++;
							if (rayPrevCellit == cellList.end())
							{
								// 終わり。
								break;
							}
							rayEndCellIt = rayPrevCellit;
							rayEndCellIt++;
						}
						else
						{
							// ぶつからなかった → 始点と終点の間のセルは除去できる。
							//cellList.erase(rayPrevCellit);
							// 次。
							rayPrevCellit = rayEndCellIt;
							rayEndCellIt++;
						}
					}
				}
#endif


				return;
			}

			bool CPathFinding::IsIntersectRayToCell(
				nsMath::CVector3 rayStartPos,
				nsMath::CVector3 rayEndPos,
				SCellWork* currentCellWork
			) const noexcept
			{
				// レイを計算する。
				nsMath::CVector3 ray = rayEndPos - rayStartPos;

				bool isVisible = false;

				// 可視ということは、rayStartPosからrayEndPosに向かって伸びる線分が、セルのどれか１つのエッジと交差しているということ。
				constexpr unsigned int vertNo[3][2] =
				{
					{0, 1},
					{1, 2},
					{2, 0},
				};

				const CNavCell* currentCell = currentCellWork->cell;
				for (int edgeNo = 0; edgeNo < 3; edgeNo++)
				{
					const nsMath::CVector3& p0 = currentCell->GetVertexPosition(vertNo[edgeNo][0]);
					const nsMath::CVector3& p1 = currentCell->GetVertexPosition(vertNo[edgeNo][1]);
					// まずは無限線分として交差しているか判定。
					nsMath::CVector3 p0ToStartPos = rayStartPos - p0;
					p0ToStartPos.y = 0.0f;
					nsMath::CVector3 p1ToEndPos = rayEndPos - p1;
					p1ToEndPos.y = 0.0f;

					// p0ToStartPosとp1ToEndPosを正規化する。
					nsMath::CVector3 p0ToStartPosNorm = p0ToStartPos;
					p0ToStartPosNorm.Normalize();
					nsMath::CVector3 p1ToEndPosNorm = p1ToEndPos;
					p1ToEndPosNorm.Normalize();

					if (p0ToStartPosNorm.Dot(p1ToEndPosNorm) <= 0.0f)
					{
						// 交差している。
						// 続いて交点を求める。
						// まずは、XZ平面でレイに垂直な線分を求める。
						nsMath::CVector3 edgeDirection = p1 - p0;
						edgeDirection.Normalize();
						nsMath::CVector3 edgeTangent;
						edgeTangent.Cross(edgeDirection, nsMath::CVector3::AxisY());
						edgeTangent.Normalize();
						float t0 = fabsf(Dot(edgeTangent, p0ToStartPos));
						float t1 = fabsf(Dot(edgeTangent, p1ToEndPos));
						// 始点から交点までの比率を求める。
						float rate = t0 / (t0 + t1);
						nsMath::CVector3 hitPos;
						hitPos.Lerp(rate, rayStartPos, rayEndPos);
						// 続いて交点が線分上にいるか調べる。
						nsMath::CVector3 rsToHitPos = hitPos - p0;
						nsMath::CVector3 reToHitPos = hitPos - p1;
						rsToHitPos.Normalize();
						reToHitPos.Normalize();
						if (rsToHitPos.Dot(reToHitPos) <= 0.0f)
						{
							// 交差している場合はこのベクトルが逆向きになるはず。
							// 交差している。
							isVisible = true;
							break;
						}
					}
				}

				return isVisible;
			}





		}
	}
}
