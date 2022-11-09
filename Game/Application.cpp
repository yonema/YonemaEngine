#include "Application.h"
#include "YonemaEngine/YonemaEngine.h"
#include "YonemaEngine/GameWindow/GameWindow.h"

namespace nsYMEngine
{
	CApplication* CApplication::m_instance = nullptr;

	const int CApplication::m_kWindowWidth = 1280;
	const int CApplication::m_kWindowHeight = 720;


	CApplication::~CApplication()
	{
		Terminate();

		return;
	}

	bool CApplication::Init()
	{
		// WIC�e�N�X�`�������[�h����ہACOM���g�����ߏ���������
		auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(result))
		{
			nsGameWindow::MessageBoxError(L"COM�̏������Ɏ��s���܂����B");
			return false;
		}

		m_gameWindow = nsGameWindow::CGameWindow::CreateInstance(m_kWindowWidth, m_kWindowHeight);

		m_ymEngine = CYonemaEngine::CreateInstance();
		if (m_ymEngine->Init() != true)
		{
			return false;
		}

		return true;
	}

	void CApplication::Terminate()
	{
		CYonemaEngine::DeleteInstance();
		nsGameWindow::CGameWindow::DeleteInstance();
		CoUninitialize();

		return;
	}

	void CApplication::Run(int nCmdShow)
	{
		m_gameWindow->DisplayWindow(nCmdShow);

		while (m_gameWindow->DispatchWindowMessage())
		{
			m_ymEngine->Update();
		}

		return;
	}
}

