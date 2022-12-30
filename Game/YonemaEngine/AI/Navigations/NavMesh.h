#pragma once
#include "NavCell.h"
#include "../../Geometries/BSPTree.h"

namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{

			class CNavMesh : private nsUtils::SNoncopyable
			{
			public:
				constexpr CNavMesh() = default;
				~CNavMesh() = default;

				bool Init(
					const char* filePath, const nsMath::CMatrix& mBias = nsMath::CMatrix::Identity());

				const CNavCell& FindNearestNavCell(const nsMath::CVector3& pos) const noexcept;

				_CONSTEXPR20_CONTAINER int GetNumCell() const noexcept
				{
					return static_cast<int>(m_navCellArray.size());
				}

				_CONSTEXPR20_CONTAINER const CNavCell& GetCell(int cellNo) const noexcept
				{
					return m_navCellArray.at(cellNo);
				}

			private:
				std::vector<CNavCell> m_navCellArray = {};
				nsGeometries::CBSPTree m_cellCenterPosBSP = {};	// セルの中心座標で構築されたBSP

			};


		}
	}
}