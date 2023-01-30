#include "UpdateAnimationManager.h"
#include "UpdateAnimationController.h"

namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsAnimations
		{
			CUpdateAnimationManager* CUpdateAnimationManager::m_instance = nullptr;

			void CUpdateAnimationManager::Update() noexcept
			{

				if (m_dispenseCounter >= m_kNumDispense)
				{
					ResetUpdateDispenser();
					m_dispenseCounter = 0;
				}


				for (auto& updateAnimController : m_updateDispenser[m_dispenseCounter])
				{
					updateAnimController->Updatable();
				}


				m_dispenseCounter++;

				return;
			}


			void CUpdateAnimationManager::ResetUpdateDispenser() noexcept
			{
				if (m_updateAnimControllerList.empty())
				{
					return;
				}

				for (unsigned int dispensIdx = 0; dispensIdx < m_kNumDispense; dispensIdx++)
				{
					m_updateDispenser[dispensIdx].clear();
				}


				unsigned int dispensIdx = 0;
				for (const auto& updateAnimController : m_updateAnimControllerList)
				{
					if (updateAnimController->GetReducingUpdate() != true)
					{
						continue;
					}

					m_updateDispenser[dispensIdx].emplace_back(updateAnimController);
					dispensIdx++;
					if (dispensIdx >= m_kNumDispense)
					{
						dispensIdx = 0;
					}
				}

				return;
			}


			void CUpdateAnimationManager::RemoveUpdateAnimController(
				CUpdateAnimationController* updateAnimControler) noexcept
			{
				// まずは、m_updateAnimControllerListの中から目的のオブジェクトを取り除く

				bool isFound = false;
				m_updateAnimControllerList.remove_if(
					[&](CUpdateAnimationController* item)
					{
						if (updateAnimControler == item)
						{
							isFound = true;
							return true;
						}
						return false;
					}
				);

				if (isFound != true)
				{
					// 無かったら終了。
					return;
				}

				// 見つかったら次は、m_updateDispenserの中から目的のオブジェクトを取り除く

				// フラグは再使用するため、リセットする。
				isFound = false;

				for (unsigned int dispensIdx = 0; dispensIdx < m_kNumDispense; dispensIdx++)
				{
					m_updateDispenser[dispensIdx].remove_if(
						[&](CUpdateAnimationController* item)
						{
							if (updateAnimControler == item)
							{
								isFound = true;
								return true;
							}
							return false;
						}
					);

					if (isFound)
					{
						// 1つでも見つかったら即座に終了する
						break;
					}
				}



				return;
			}




		}
	}
}