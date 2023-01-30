#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CUpdateAnimationController;
		}
	}
}
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			class CUpdateAnimationManager : private nsUtils::SNoncopyable
			{
			public:
				static const unsigned int m_kNumDispense = 8;

			private:
				constexpr CUpdateAnimationManager() = default;
				~CUpdateAnimationManager() = default;

			public:
				static CUpdateAnimationManager* CreateInstance()
				{
					if (m_instance == nullptr)
					{
						m_instance = new CUpdateAnimationManager();
					}

					return m_instance;
				}

				static constexpr CUpdateAnimationManager* GetInstance() noexcept
				{
					return m_instance;
				}

				static void DeleteInstance()
				{
					if (m_instance)
					{
						delete m_instance;
						m_instance = nullptr;
					}
				}

				void Update() noexcept;

				void AddUpdateAnimController(CUpdateAnimationController* updateAnimController) noexcept
				{
					m_updateAnimControllerList.emplace_back(updateAnimController);
				}

				void RemoveUpdateAnimController(CUpdateAnimationController* updateAnimControler) noexcept;

			private:

				void ResetUpdateDispenser() noexcept;


			private:
				static CUpdateAnimationManager* m_instance;
				std::list<CUpdateAnimationController*> m_updateAnimControllerList = {};
				std::array<std::list<CUpdateAnimationController*>, m_kNumDispense>
					m_updateDispenser = {};
				unsigned int m_dispenseCounter = 0;

			};

		}
	}
}