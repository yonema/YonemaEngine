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
						// ���_���W��ݒ�
						const auto index = mesh->mFaces[faceIdx].mIndices[vertIdx];
						const auto& aiVertPos = mesh->mVertices[index];
						nsMath::CVector3 vertPos(aiVertPos.x, aiVertPos.y, aiVertPos.z);
						mBias.Apply(vertPos);
						navCell.SetVertexPosition(vertIdx, vertPos);

						// ���_�@������Z���̖ʖ@�����v�Z
						nsMath::CVector3 vertNormal(
							mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);
						faceNormal += vertNormal;

						// ���_���W����Z���̒��S���W���v�Z
						centerPos += vertPos;
					}

					// �@����ݒ�
					faceNormal.Normalize();
					navCell.SetNormal(faceNormal);

					// ���S���W��ݒ�
					centerPos /= 3.0f;
					navCell.SetCenterPosition(centerPos);

					// �Z���ԍ���ݒ�
					navCell.SetNo(faceIdx);

					// �אڃZ����ݒ�
					for (unsigned int linkIdx = faceIdx + 1; linkIdx < kNumFaces; linkIdx++)
					{
						// �o�u���\�[�g�ŗאڃZ����T��

						int count = 0;
						// ���g�̃Z���̒��_�C���f�b�N�X��1�����ׂ�
						for (unsigned int fvIdx = 0; fvIdx < 3; fvIdx++)
						{
							// ����̃Z���̒��_�C���f�b�N�X��1�����ׂ�
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
							// ���_�C���f�b�N�X��2������������A�אڂ��Ă���B
							auto& linkNavCell = m_navCellArray.at(linkIdx);

							navCell.AddLinkSell(&linkNavCell);
							linkNavCell.AddLinkSell(&navCell);
						}

					}



				}

				// BSP�c���[�\�z

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
							//������̕����߂��B
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