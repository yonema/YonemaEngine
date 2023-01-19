#include "AABB.h"
#include "../Graphics/Models/BasicModelRenderer.h"

namespace nsYMEngine
{
	namespace nsGeometries
	{
		void CAABB::Init(const nsMath::CVector3& vMax, const nsMath::CVector3& vMin) noexcept
		{
			nsMath::CVector3 halfSize = (vMax - vMin) * 0.5f;
			nsMath::CVector3 centerPos = (vMax + vMin) * 0.5f;

			constexpr unsigned int X0Y0Z0 = static_cast<unsigned int>(EnVertexPos::enX0Y0Z0);	// ������O
			constexpr unsigned int X0Y0Z1 = static_cast<unsigned int>(EnVertexPos::enX0Y0Z1);	// ������
			constexpr unsigned int X0Y1Z0 = static_cast<unsigned int>(EnVertexPos::enX0Y1Z0);	// �����O
			constexpr unsigned int X0Y1Z1 = static_cast<unsigned int>(EnVertexPos::enX0Y1Z1);	// ���㉜
			constexpr unsigned int X1Y0Z0 = static_cast<unsigned int>(EnVertexPos::enX1Y0Z0);	// �E����O
			constexpr unsigned int X1Y0Z1 = static_cast<unsigned int>(EnVertexPos::enX1Y0Z1);	// �E����
			constexpr unsigned int X1Y1Z0 = static_cast<unsigned int>(EnVertexPos::enX1Y1Z0);	// �E���O
			constexpr unsigned int X1Y1Z1 = static_cast<unsigned int>(EnVertexPos::enX1Y1Z1);	// �E�㉜

			// ������O
			m_vertexPosition[X0Y0Z0] = centerPos;
			m_vertexPosition[X0Y0Z0].x -= halfSize.x;
			m_vertexPosition[X0Y0Z0].y -= halfSize.y;
			m_vertexPosition[X0Y0Z0].z -= halfSize.z;

			// ������
			m_vertexPosition[X0Y0Z1] = centerPos;
			m_vertexPosition[X0Y0Z1].x -= halfSize.x;
			m_vertexPosition[X0Y0Z1].y -= halfSize.y;
			m_vertexPosition[X0Y0Z1].z += halfSize.z;

			// �����O
			m_vertexPosition[X0Y1Z0] = centerPos;
			m_vertexPosition[X0Y1Z0].x -= halfSize.x;
			m_vertexPosition[X0Y1Z0].y += halfSize.y;
			m_vertexPosition[X0Y1Z0].z -= halfSize.z;

			// ���㉜
			m_vertexPosition[X0Y1Z1] = centerPos;
			m_vertexPosition[X0Y1Z1].x -= halfSize.x;
			m_vertexPosition[X0Y1Z1].y += halfSize.y;
			m_vertexPosition[X0Y1Z1].z += halfSize.z;

			// �E����O
			m_vertexPosition[X1Y0Z0] = centerPos;
			m_vertexPosition[X1Y0Z0].x += halfSize.x;
			m_vertexPosition[X1Y0Z0].y -= halfSize.y;
			m_vertexPosition[X1Y0Z0].z -= halfSize.z;

			// �E����
			m_vertexPosition[X1Y0Z1] = centerPos;
			m_vertexPosition[X1Y0Z1].x += halfSize.x;
			m_vertexPosition[X1Y0Z1].y -= halfSize.y;
			m_vertexPosition[X1Y0Z1].z += halfSize.z;

			// �E���O
			m_vertexPosition[X1Y1Z0] = centerPos;
			m_vertexPosition[X1Y1Z0].x += halfSize.x;
			m_vertexPosition[X1Y1Z0].y += halfSize.y;
			m_vertexPosition[X1Y1Z0].z -= halfSize.z;

			// �E�㉜
			m_vertexPosition[X1Y1Z1] = centerPos;
			m_vertexPosition[X1Y1Z1].x += halfSize.x;
			m_vertexPosition[X1Y1Z1].y += halfSize.y;
			m_vertexPosition[X1Y1Z1].z += halfSize.z;

			return;
		}

		void CAABB::InitFromMeshes(const std::vector<SMesh>& meshes) noexcept
		{
			nsMath::CVector3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			nsMath::CVector3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);

			for (const auto& mesh : meshes)
			{
				for (const auto& vertex : mesh.vertices)
				{
					vMax.Max(vertex.position);
					vMin.Min(vertex.position);
				}
			}

			Init(vMax, vMin);

			return;
		}

		void CAABB::CalcVertexPositions(
			nsMath::CVector3 posOut[m_kNumVertesPos],
			const nsMath::CMatrix& mWorld
		) const noexcept
		{
			for (unsigned int vertIdx = 0; vertIdx < m_kNumVertesPos; vertIdx++)
			{
				posOut[vertIdx] = m_vertexPosition[vertIdx];
				mWorld.Apply(posOut[vertIdx]);
			}

			return;
		}
	}
}
