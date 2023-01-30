#pragma once
#include "AABB.h"
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
		class CGeometryData : private nsUtils::SNoncopyable
		{
		private:
			using SMesh = nsGraphics::nsModels::SMesh;

		public:
			constexpr CGeometryData() = default;
			~CGeometryData() = default;

			void Init(const std::vector<SMesh>& meshes) noexcept;

			void Init(const CAABB& aabb) noexcept
			{
				m_aabb.InitFormAABB(aabb);
			}

			void Update(const nsMath::CMatrix& mWorld) noexcept;

			constexpr bool IsInViewFrustum() const noexcept
			{
				return m_isInViewFrustum;
			}

			constexpr const CAABB& GetAABB() const noexcept
			{
				return m_aabb;
			}

			static void DebugUpdate(float deltaTime);

		private:
			static void DrawDebugLine(
				const nsMath::CVector3* vertexPos, const nsMath::CVector4& color);

			static void CalcDebugViewProjMat(float deltaTime);

			static void DrawCameraFrustum(
				const nsMath::CVector3& cameraPos,
				const nsMath::CVector3& cameraForward,
				const nsMath::CVector3& cameraUp,
				const nsMath::CVector3& cameraRight,
				float nearDepth,
				float farDepth,
				float fovAngleY,
				float aspectRatio
			);

		private:
			CAABB m_aabb = {};
			bool m_isInViewFrustum = false;
#ifdef _DEBUG
			static nsMath::CMatrix m_mViewProjDebug;
			static bool m_isDebugDraw;
#endif // _DEBUG

		};

	}
}