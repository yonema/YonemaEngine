#include "NavMesh.h"
#include "../Graphics/Models/AssimpCommon.h"


namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{

			bool CNavMesh::Init(const char* filePath, const nsMath::CMatrix& mBias)
			{
				Assimp::Importer* importer = nullptr;
				const aiScene* scene = nullptr;

				if (nsGraphics::nsAssimpCommon::ImportScene(
					filePath,
					importer,
					scene,
					nsGraphics::nsAssimpCommon::g_kNavMeshRemoveComponentFlags,
					nsGraphics::nsAssimpCommon::g_kNavMeshPostprocessFlags
				) != true)
				{
					return false;
				}


				const auto kNumFaces = scene->mMeshes[0]->mNumFaces;
				m_navCellArray.resize(kNumFaces);

				for (unsigned int faceIdx = 0; faceIdx < kNumFaces; faceIdx++)
				{
					CNavCell& navCell = m_navCellArray.at(faceIdx);

					const auto& mesh = scene->mMeshes[0];

					nsMath::CVector3 faceNormal = nsMath::CVector3::Zero();
					nsMath::CVector3 centerPos = nsMath::CVector3::Zero();

					for (unsigned int vertIdx = 0; vertIdx < 3; vertIdx++)
					{
						// 頂点座標を設定
						const auto index = mesh->mFaces[faceIdx].mIndices[vertIdx];
						const auto& aiVertPos = mesh->mVertices[index];
						nsMath::CVector3 vertPos(aiVertPos.x, aiVertPos.y, aiVertPos.z);
						mBias.Apply(vertPos);
						navCell.SetVertexPosition(vertIdx, vertPos);

						// 頂点法線からセルの面法線を計算
						nsMath::CVector3 vertNormal(
							mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);
						faceNormal += vertNormal;

						// 頂点座標からセルの中心座標を計算
						centerPos += vertPos;
					}

					// 法線を設定
					faceNormal.Normalize();
					navCell.SetNormal(faceNormal);

					// 中心座標を設定
					centerPos /= 3.0f;
					navCell.SetCenterPosition(centerPos);

					// セル番号を設定
					navCell.SetNo(faceIdx);

					// 隣接セルを設定
					for (unsigned int linkIdx = faceIdx + 1; linkIdx < kNumFaces; linkIdx++)
					{
						// バブルソートで隣接セルを探す

						int count = 0;
						// 自身のセルの頂点インデックスを1つずつ調べる
						for (unsigned int fvIdx = 0; fvIdx < 3; fvIdx++)
						{
							// 相手のセルの頂点インデックスを1つずつ調べる
							for (unsigned int lvIdx = 0; lvIdx < 3; lvIdx++)
							{
								if (scene->mMeshes[0]->mFaces[faceIdx].mIndices[fvIdx] ==
									scene->mMeshes[0]->mFaces[linkIdx].mIndices[lvIdx])
								{
									count++;
									break;
								}
							}
						}

						if (count == 2)
						{
							// 頂点インデックスが2つ同じだったら、隣接している。
							auto& linkNavCell = m_navCellArray.at(linkIdx);

							navCell.AddLinkSell(&linkNavCell);
							linkNavCell.AddLinkSell(&navCell);
						}

					}



				}

				// BSPツリー構築

				m_cellCenterPosBSP.ReserveLeaf(kNumFaces);
				for (auto& cell : m_navCellArray)
				{
					m_cellCenterPosBSP.AddLeaf(cell.GetCenterPosition(), &cell);
				}

				m_cellCenterPosBSP.Build();


				return true;

			}

			const CNavCell& CNavMesh::FindNearestNavCell(const nsMath::CVector3& pos) const noexcept
			{
				const CNavCell* nearestCell = nullptr;

				float dist = FLT_MAX;
				m_cellCenterPosBSP.WalkTree(
					pos,
					[&](nsGeometries::CBSPTree::SLeaf* leaf)
					{
						CNavCell* cell = static_cast<CNavCell*>(leaf->extraData);
						float distTmp = (cell->GetCenterPosition() - pos).Length();
						if (distTmp < dist)
						{
							//こちらの方が近い。
							dist = distTmp;
							nearestCell = cell;
						}
					}
				);

				return *nearestCell;
			}

		}
	}
}