#pragma once

namespace nsYMEngine
{
	namespace nsWindows
	{
		class CWindows
		{
		public:
			CWindows(const int kWindowWidth, const int kWindowHeight);
			~CWindows();

			HWND GetHWND() 
			{
				return m_hwnd;
			}
		private:
			HWND m_hwnd;
			WNDCLASSEX m_windClassEx;
		};
	}
}