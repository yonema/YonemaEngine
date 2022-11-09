#include "Keyboard.h"

namespace nsYMEngine
{
	namespace nsInput
	{
		const int CKeyboard::m_kByteSizeForGetKeyboardState = 256;
		const DWORD CKeyboard::m_kKeycodeTable[static_cast<int>(EnKeyButton::enNumButtons)]
		{
			0,
			// A to Z
			'A',	// enA,
			'B',	// enB,
			'C',	// enC,
			'D',	// enD,
			'E',	// enE,
			'F',	// enF,
			'G',	// enG,
			'H',	// enH,
			'I',	// enI,
			'J',	// enJ,
			'K',	// enK,
			'L',	// enL,
			'M',	// enM,
			'N',	// enN,
			'O',	// enO,
			'P',	// enP,
			'Q',	// enQ,
			'R',	// enR,
			'S',	// enS,
			'T',	// enT,
			'U',	// enU,
			'V',	// enV,
			'W',	// enW,
			'X',	// enX,
			'Y',	// enY,
			'Z',	// enZ,

			// 0 to 9
			'0',	// en0,
			'1',	// en1,
			'2',	// en2,
			'3',	// en3,
			'4',	// en4,
			'5',	// en5,
			'6',	// en6,
			'7',	// en7,
			'8',	// en8,
			'9',	// en9,

			// numpad 0 to 9
			VK_NUMPAD0,		// enNumpad0,
			VK_NUMPAD1,		// enNumpad1,
			VK_NUMPAD2,		// enNumpad2,
			VK_NUMPAD3,		// enNumpad3,
			VK_NUMPAD4,		// enNumpad4,
			VK_NUMPAD5,		// enNumpad5,
			VK_NUMPAD6,		// enNumpad6,
			VK_NUMPAD7,		// enNumpad7,
			VK_NUMPAD8,		// enNumpad8,
			VK_NUMPAD9,		// enNumpad9,

			// arrow key
			VK_UP,		// enUp,
			VK_DOWN,	// enDown,
			VK_LEFT,	// enLeft,
			VK_RIGHT,	// enRight,

			// Other
			VK_RETURN,		// enEnter,
			VK_SPACE,		// enSpace,
			VK_ESCAPE,		// enEscape,
			VK_TAB,			// enTab,
			VK_SHIFT,		// enShift,
			VK_LSHIFT,		// enLShift,
			VK_RSHIFT,		// enRShift,
			VK_CONTROL,		// enCtrl,
			VK_LCONTROL,	// enLCtrl,
			VK_RCONTROL,	// enRCtrl,
			VK_MENU,		// enAlt,
			VK_LMENU,		// enLAlt,
			VK_RMENU,		// enRAlt,
		};


		void CKeyboard::Update()
		{
			BYTE key[m_kByteSizeForGetKeyboardState];
			GetKeyboardState(key);

			for (int i = 0; i < static_cast<int>(EnKeyButton::enNumButtons); i++)
			{
				if (m_kKeycodeTable[i] == 0)
				{
					// enNoneはとばす。
					continue;
				}

				if (key[m_kKeycodeTable[i]] & 0x80)
				{
					// 対象のボタンが押されている。
					// triggerは、ボタンが押された最初の1フレームだけ1にする。
					// pressは、ボタンが押されている間ずっと1にする。
					m_inputState.triggerArray[i] = 1 ^ m_inputState.pressArray[i];
					m_inputState.pressArray[i] = 1;
				}
				else
				{
					// 対象のボタンが押されていない。
					m_inputState.triggerArray[i] = 0;
					m_inputState.pressArray[i] = 0;
				}
			}

			return;
		}
	}
}