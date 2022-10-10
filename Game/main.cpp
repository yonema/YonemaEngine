#include "YonemaEngine/GameWindow/GameWindow.h"
#include "YonemaEngine/GameWindow/MessageBox.h"
#include "Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	nsYMEngine::CApplication::CreateInstance();
	auto app = nsYMEngine::CApplication::GetInstance();
	bool res = app->Init();

	if (res)
	{
		app->Run(nCmdShow);
	}

	nsYMEngine::CApplication::DeleteInstance();

	return res ? 0 : -1;
}