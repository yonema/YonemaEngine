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
				// �p�X���N���A�B
				path.Clear();
				// �J�n�ʒu�ɍł��߂��Z���������B
				const CNavCell& startCell = naviMesh.FindNearestNavCell(startPos);
				// �I���ʒu�ɍł��߂��Z���������B
				const CNavCell& endCell = naviMesh.FindNearestNavCell(endPos);
				// A*���������ɁA�Z���ɂ��낢��ȏ��𖄂ߍ��ނ̂ŁA���𖄂ߍ��ލ�Ɨ̈���m�ہB
				m_cellWorkArray.resize(naviMesh.GetNumCell());

				// ��Ɨ̈������������B
				for (int cellNo = 0; cellNo < m_cellWorkArray.size(); cellNo++)
				{
					m_cellWorkArray[cellNo].Init(&naviMesh.GetCell(cellNo));
				}
				// �Ō�Ƀp�X�����Ƃ��̍��W��ݒ肷��B
				// �J�n�Z���͊J�n���W�A�I���Z���͏I�����W�B
				// ����ȊO�̃Z���́A�Z���̒��S���W��CellWork::Init()�֐��Őݒ肳��Ă���B
				m_cellWorkArray[startCell.GetCellNo()].pathPoint = startPos;
				m_cellWorkArray[endCell.GetCellNo()].pathPoint = endPos;

				using namespace std;
				list<SCellWork*> openList;
				// �J�n�Z���̍�Ɨ̈���擾�B
				SCellWork* crtCell = &m_cellWorkArray.at(startCell.GetCellNo());
				// �J�n�Z���̈ړ��R�X�g���v�Z����B
				CalcCost(crtCell->cost, crtCell->costFromStartCell, crtCell, nullptr, &endCell);
				// �J�n�Z�����I�[�v�����X�g�ɐςށB
				openList.emplace_back(crtCell);
				// �Z�����I�[�v�����X�g�ɐς񂾈������B
				crtCell->isOpend = true;

				// ��������A*�A���S���Y���������Ă������B
				bool isSuccess = false;
				while (!openList.empty())
				{
					// �I�[�v�����X�g����ł��R�X�g�̈����Z�����擾�B
					crtCell = nullptr;
					float lowestCost = FLT_MAX;
					list<SCellWork*>::iterator lowestCellIt;
					for (auto it = openList.begin(); it != openList.end(); it++)
					{
						if (lowestCost > (*it)->cost) 
						{
							// ���̃Z���̕��������B
							lowestCost = (*it)->cost;
							crtCell = (*it);
							lowestCellIt = it;
						}
					}
					// ���o�����Z�����I�[�v�����X�g����폜�B
					openList.erase(lowestCellIt);

					if (crtCell->cell == &endCell)
					{
						// �I�[�v�����X�g������o�����Z�����I������Ȃ�o�H�T�������B
						isSuccess = true;
						break;
					}

					// �J�����Z���̗אڃZ�����I�[�v�����X�g�ɐς�ł����B
					for (int linkCellNo = 0; linkCellNo < 3; linkCellNo++)
					{
						CNavCell* linkCell = crtCell->cell->GetLinkCell(linkCellNo);
						if (linkCell != nullptr)
						{
							// �אڃZ�����������̂ŁA�R�X�g���v�Z���ăI�[�v�����X�g�ɐςށB
							SCellWork* linkCellWork = &m_cellWorkArray.at(linkCell->GetCellNo());

							//�אڃZ���Ɉړ�����R�X�g���v�Z
							float newCost;
							float newCostFromStartCell;
							CalcCost(newCost, newCostFromStartCell, linkCellWork, crtCell, &endCell);

							if (linkCellWork->isClosed == false)
							{
								//�אڃZ���̒����͏I����Ă��Ȃ��B

								if (linkCellWork->cost > newCost)
								{
									// ������̃R�X�g�̕��������̂ŉ��P����B
									linkCellWork->costFromStartCell = newCostFromStartCell;
									linkCellWork->cost = newCost;
									// �e��ݒ�B
									linkCellWork->parentCell = crtCell;
								}
								if (linkCellWork->isOpend == false)
								{
									// �܂��I�[�v�����X�g�ɐςށB
									linkCellWork->isOpend = true;
									openList.emplace_back(linkCellWork);
								}
							}

						}
					}
					// ���̃Z���̒����͏I���Ȃ̂ŁA�N���[�Y�̈������B
					crtCell->isClosed = true;
				}

				if (isSuccess)
				{
					// �p�X���\�z�B
					list<SCellWork*> cellList;
					crtCell = &m_cellWorkArray.at(endCell.GetCellNo());
					while (true)
					{
						if (crtCell->cell == &startCell)
						{
							// �J�n�Z���ɓ��B�����B
							// �Ō�ɊJ�n�Z����ςށB
							cellList.emplace_back(crtCell);
							break;
						}
						cellList.emplace_back(crtCell);
						crtCell = crtCell->parentCell;
					}
					// ���]������B
					std::reverse(cellList.begin(), cellList.end());
					//
					Smoothing(cellList, enablePhysics, agentRadius, agentHeight);

					// �|�C���g���p�X�ɐς�ł���
					path.ReservePoint(static_cast<unsigned int>(cellList.size()));
					for (auto it = cellList.begin(); it != cellList.end(); it++)
					{
						path.AddPoint((*it)->pathPoint);
					}

					// �p�X���\�z����B
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
					// ���O�̃Z�����Ȃ��B
					costFromStartCell = 0.0f;
				}
				else
				{
					// ���O�̃Z��������B
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
				// �p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
				if (cellList.size() < 3)
				{
					// �Z���̐���3�ȉ��Ȃ�X���[�W���O����K�v�Ȃ��B
					return;
				}

				int skipCellCount = static_cast<int>(cellList.size()) - 1;
				while (skipCellCount > 2)
				{
					// �Z���̐����R�ȏ�Ȃ�A�p�X�̉�������s���āA�s�v�ȃZ�������O���Ă����B
					// ���C�̎n�_�ƂȂ�Z���B
					auto rayStartCellIt = cellList.begin();
					// ���C�̏I�_�̃Z���B
					auto rayEndCellIt = rayStartCellIt;
					for (int i = 0; i < skipCellCount; i++)
					{
						rayEndCellIt++;
					}
					bool isEnd = false;
					while (isEnd == false)
					{
						// ���C�̏I�[�̃Z���܂Ń��C�e�X�g��XZ���ʂōs���B
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
							// ��
							// �n�_����I�_��������Ƃ������Ƃ́A�n�_�`�I�_�܂ł̊Ԃ̃Z���͍폜�ł���̂ŃX���[�X�}�[�N��t����B
							auto cellIt = rayStartCellIt;
							cellIt++;
							while (cellIt != rayEndCellIt)
							{
								(*cellIt)->isSmooth = true;
								cellIt++;
							}
						}
						// ���B
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

				// �X���[�X�t���O�������Ă���Z�����������Ă���
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

				// �����I�u�W�F�N�g�Ƃ̏Փ˔�����s���A�폜�ł���Z���͍폜����B
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
							// �����̃I�u�W�F�N�g�ƂԂ������B
							// �n�_����I�_�̊Ԃ̃Z���͍폜�ł��Ȃ��B
							// ���̃u���b�N�����Ă����B
							rayStartCellIt = rayEndCellIt;
							rayPrevCellit = rayEndCellIt;
							rayPrevCellit++;
							if (rayPrevCellit == cellList.end())
							{
								// �I���B
								break;
							}
							rayEndCellIt = rayPrevCellit;
							rayEndCellIt++;
						}
						else
						{
							// �Ԃ���Ȃ����� �� �n�_�ƏI�_�̊Ԃ̃Z���͏����ł���B
							//cellList.erase(rayPrevCellit);
							// ���B
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
				// ���C���v�Z����B
				nsMath::CVector3 ray = rayEndPos - rayStartPos;

				bool isVisible = false;

				// ���Ƃ������Ƃ́ArayStartPos����rayEndPos�Ɍ������ĐL�т�������A�Z���̂ǂꂩ�P�̃G�b�W�ƌ������Ă���Ƃ������ƁB
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
					// �܂��͖��������Ƃ��Č������Ă��邩����B
					nsMath::CVector3 p0ToStartPos = rayStartPos - p0;
					p0ToStartPos.y = 0.0f;
					nsMath::CVector3 p1ToEndPos = rayEndPos - p1;
					p1ToEndPos.y = 0.0f;

					// p0ToStartPos��p1ToEndPos�𐳋K������B
					nsMath::CVector3 p0ToStartPosNorm = p0ToStartPos;
					p0ToStartPosNorm.Normalize();
					nsMath::CVector3 p1ToEndPosNorm = p1ToEndPos;
					p1ToEndPosNorm.Normalize();

					if (p0ToStartPosNorm.Dot(p1ToEndPosNorm) <= 0.0f)
					{
						// �������Ă���B
						// �����Č�_�����߂�B
						// �܂��́AXZ���ʂŃ��C�ɐ����Ȑ��������߂�B
						nsMath::CVector3 edgeDirection = p1 - p0;
						edgeDirection.Normalize();
						nsMath::CVector3 edgeTangent;
						edgeTangent.Cross(edgeDirection, nsMath::CVector3::AxisY());
						edgeTangent.Normalize();
						float t0 = fabsf(Dot(edgeTangent, p0ToStartPos));
						float t1 = fabsf(Dot(edgeTangent, p1ToEndPos));
						// �n�_�����_�܂ł̔䗦�����߂�B
						float rate = t0 / (t0 + t1);
						nsMath::CVector3 hitPos;
						hitPos.Lerp(rate, rayStartPos, rayEndPos);
						// �����Č�_��������ɂ��邩���ׂ�B
						nsMath::CVector3 rsToHitPos = hitPos - p0;
						nsMath::CVector3 reToHitPos = hitPos - p1;
						rsToHitPos.Normalize();
						reToHitPos.Normalize();
						if (rsToHitPos.Dot(reToHitPos) <= 0.0f)
						{
							// �������Ă���ꍇ�͂��̃x�N�g�����t�����ɂȂ�͂��B
							// �������Ă���B
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
