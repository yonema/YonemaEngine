#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsModels
		{
			struct SMesh;
		}
	}
}

namespace nsYMEngine
{
	namespace nsGeometries
	{
		class CAABB : private nsUtils::SNoncopyable
		{
		public:
			enum class EnVertexPos
			{
				enX0Y0Z0,	// ������O
				enX0Y0Z1,	// ������
				enX0Y1Z0,	// �����O
				enX0Y1Z1,	// ���㉜
				enX1Y0Z0,	// �E����O
				enX1Y0Z1,	// �E����
				enX1Y1Z0,	// �E���O
				enX1Y1Z1,	// �E�㉜
				enNumVertexPos
			};

			static const unsigned int m_kNumVertesPos = 
				static_cast<int>(EnVertexPos::enNumVertexPos);

		private:
			using SMesh = nsGraphics::nsModels::SMesh;

		public:
			constexpr CAABB() = default;
			~CAABB() = default;

			void Init(const nsMath::CVector3& vMax, const nsMath::CVector3& vMin) noexcept;

			void InitFromMeshes(const std::vector<SMesh>& meshes) noexcept;

			void InitFormAABB(const CAABB& aabb)
			{
				const auto* other = aabb.GetVertexPosition();
				for (unsigned int vertIdx = 0; vertIdx < m_kNumVertesPos; vertIdx++)
				{
					m_vertexPosition[vertIdx] = other[vertIdx];
				}
			}

			void CalcVertexPositions(
				nsMath::CVector3 posOut[m_kNumVertesPos],
				const nsMath::CMatrix& mWorld
			) const noexcept;

			constexpr const nsMath::CVector3* GetVertexPosition() const noexcept
			{
				return m_vertexPosition;
			}

		private:
			nsMath::CVector3 m_vertexPosition[m_kNumVertesPos];
		};

	}
}