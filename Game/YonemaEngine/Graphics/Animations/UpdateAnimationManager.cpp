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
				// �܂��́Am_updateAnimControllerList�̒�����ړI�̃I�u�W�F�N�g����菜��

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
					// ����������I���B
					return;
				}

				// ���������玟�́Am_updateDispenser�̒�����ړI�̃I�u�W�F�N�g����菜��

				// �t���O�͍Ďg�p���邽�߁A���Z�b�g����B
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
						// 1�ł����������瑦���ɏI������
						break;
					}
				}



				return;
			}




		}
	}
}