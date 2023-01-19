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

		private:
			CAABB m_aabb = {};
			bool m_isInViewFrustum = false;
		};

	}
}