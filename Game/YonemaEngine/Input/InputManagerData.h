#pragma once
namespace nsYMEngine
{
	namespace nsInput
	{
		namespace nsInputManagerData
		{
			/**
			 * @brief ゲームパッドの軸の種類
			*/
			enum class EnGamepadStickAxis
			{
				enLStickX,
				enLStickY,
				enRStickX,
				enRStickY,
				enNumStickAxes
			};

			/**
			 * @brief アクションマッピングの1パーツ
			*/
			struct SActionMapChip
			{
				std::vector<CGamepad::EnPadButton> gamepadButtonTable;
				std::vector<CKeyboard::EnKeyButton> keyboardButtontable;
			};

			/**
			 * @brief アクシズマッピングの1パーツ
			*/
			struct SAxisMapChip
			{
				std::vector<std::pair<CGamepad::EnPadButton, float>> gamepadButtonTable;
				std::vector<std::pair<CKeyboard::EnKeyButton, float>> keyboardButtontable;
				std::vector<std::pair<EnGamepadStickAxis, float>> gamepadStickTable;
			};
		}
	}
}