#pragma once
namespace nsYMEngine
{
	namespace nsInput
	{
		class CKeyboard
		{
		public:
			/**
			 * @brief キーボードの仮想ボタン
			*/
			enum class EnKeyButton
			{
				enNone,
				// A to Z
				enA,
				enB,
				enC,
				enD,
				enE,
				enF,
				enG,
				enH,
				enI,
				enJ,
				enK,
				enL,
				enM,
				enN,
				enO,
				enP,
				enQ,
				enR,
				enS,
				enT,
				enU,
				enV,
				enW,
				enX,
				enY,
				enZ,

				// 0 to 9
				en0,
				en1,
				en2,
				en3,
				en4,
				en5,
				en6,
				en7,
				en8,
				en9,

				// numpad 0 to 9
				enNumpad0,
				enNumpad1,
				enNumpad2,
				enNumpad3,
				enNumpad4,
				enNumpad5,
				enNumpad6,
				enNumpad7,
				enNumpad8,
				enNumpad9,

				// arrow key
				enUp,
				enDown,
				enLeft,
				enRight,

				// Other
				enEnter,
				enSpace,
				enEscape,
				enTab,
				enShift,
				enLShift,
				enRShift,
				enCtrl,
				enLCtrl,
				enRCtrl,
				enAlt,
				enLAlt,
				enRAlt,

				enNumButtons
			};

			struct SInputState
			{
				std::bitset<static_cast<int>(EnKeyButton::enNumButtons)> pressArray;
				std::bitset<static_cast<int>(EnKeyButton::enNumButtons)> triggerArray;
			};

		private:
			static const int m_kByteSizeForGetKeyboardState;
			static const DWORD m_kKeycodeTable[static_cast<int>(EnKeyButton::enNumButtons)];


		public:
			constexpr CKeyboard() = default;
			~CKeyboard() = default;

			void Update();


			constexpr const auto& GetInputState() const noexcept
			{
				return m_inputState;
			}

			constexpr bool IsTrigger(EnKeyButton keyButton) const noexcept
			{
				return m_inputState.triggerArray[static_cast<int>(keyButton)];
			}
			constexpr bool IsPress(EnKeyButton keyButton) const noexcept
			{
				return m_inputState.pressArray[static_cast<int>(keyButton)];
			}
			inline bool IsPressAnyKey() const noexcept
			{
				return m_inputState.pressArray.any();
			}


		private:
			SInputState m_inputState = {};
		};

	}
}