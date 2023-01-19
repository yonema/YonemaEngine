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
				enX0Y0Z0,	// 左下手前
				enX0Y0Z1,	// 左下奥
				enX0Y1Z0,	// 左上手前
				enX0Y1Z1,	// 左上奥
				enX1Y0Z0,	// 右下手前
				enX1Y0Z1,	// 右下奥
				enX1Y1Z0,	// 右上手前
				enX1Y1Z1,	// 右上奥
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