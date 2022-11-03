#include "YonemaEnginePreCompile.h"
#include "Game.h"
#include "../YonemaEngine.h"

namespace nsAWA
{
	const char* const CGame::m_kGameObjectName = "Game";

	bool CGame::Start()
	{

		m_debugNum++;

		int a = 0;

		return true;
	}

	void CGame::Update(float deltaTime)
	{
		m_debugNum++;

		int a = 0;


		if (m_debugNum >= 1000)
		{
			CYonemaEngine::GetInstance()->ExitGame();
		}



		return;
	}

	void CGame::OnDestroy()
	{
		m_debugNum = 0;
		int a = 0;

		return;
	}


}
