#pragma once
namespace nsYMEngine
{
	namespace nsAI
	{
		namespace nsNavigations
		{

			class CNavCell
			{
			public:
				constexpr CNavCell() = default;
				~CNavCell() = default;

				inline void SetVertexPosition(unsigned int vertNo, const nsMath::CVector3& pos) noexcept
				{
#ifdef _DEBUG
					if (vertNo >= 3)
					{
						nsGameWindow::MessageBoxError(L"ナビメッシュの頂点数が規定数より多いです。");
						return;
					}
#endif // _DEBUG
					m_vertexPosition[vertNo] = pos;
				}

				constexpr const nsMath::CVector3& GetVertexPosition(unsigned int vertNo) const noexcept
				{
					return m_vertexPosition[vertNo];
				}

				inline void SetNormal(const nsMath::CVector3& normal) noexcept
				{
					m_normal = normal;
				}

				inline void SetCenterPosition(const nsMath::CVector3& centerPos) noexcept
				{
					m_centerPos = centerPos;
				}

				constexpr const nsMath::CVector3& GetCenterPosition() const noexcept
				{
					return m_centerPos;
				}

				inline void AddLinkSell(CNavCell* navCell) noexcept
				{
					for (int i = 0; i < 3; i++)
					{
						if (m_linkNavCell[i] == nullptr)
						{
							m_linkNavCell[i] = navCell;
							break;
						}

#ifdef _DEBUG
						if (i >= 2)
						{
							nsGameWindow::MessageBoxError(L"ナビメッシュの隣接セルが規定数より多いです。");
						}
#endif // _DEBUG

					}
				}

				constexpr CNavCell* GetLinkCell(int linkCellNo) const noexcept
				{
					return m_linkNavCell[linkCellNo];
				}

				constexpr void SetNo(unsigned int no) noexcept
				{
					m_no = no;
				}

				constexpr int GetCellNo() const noexcept
				{
					return m_no;
				}

			private:
				nsMath::CVector3 m_vertexPosition[3];
				nsMath::CVector3 m_normal = nsMath::CVector3::Up();
				nsMath::CVector3 m_centerPos = nsMath::CVector3::Zero();
				CNavCell* m_linkNavCell[3] = { nullptr };
				unsigned int m_no = 0;
			};

		}
	}
}
