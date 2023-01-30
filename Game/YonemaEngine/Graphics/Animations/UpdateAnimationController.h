#pragma once

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CUpdateAnimationController : private nsUtils::SNoncopyable
			{
			public:
				CUpdateAnimationController();


				~CUpdateAnimationController();

				constexpr void Init(float distToReducingUpdate) noexcept
				{
					m_distanceToReducingUpdate = distToReducingUpdate;
				}

				void Update(const nsMath::CVector3& pos);


				constexpr bool GetReducingUpdate() const noexcept
				{
					return m_isReducingUpdate;
				}

				constexpr void Updatable() noexcept
				{
					m_isUpdatable = true;
				}

				constexpr void NotUpdatable() noexcept
				{
					m_isUpdatable = false;
				}

				constexpr bool IsUpdatable() const noexcept
				{
					return m_isUpdatable;
				}


			private:
				float m_distanceToReducingUpdate = -1.0f;
				bool m_isUpdatable = true;
				bool m_isReducingUpdate = false;
			};

		}
	}
}