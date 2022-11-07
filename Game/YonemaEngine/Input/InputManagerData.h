#pragma once
namespace nsYMEngine
{
	namespace nsInput
	{
		namespace nsInputManagerData
		{
			/**
			 * @brief �Q�[���p�b�h�̎��̎��
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
			 * @brief �A�N�V�����}�b�s���O��1�p�[�c
			*/
			struct SActionMapChip
			{
				std::vector<CGamepad::EnPadButton> gamepadButtonTable;
				std::vector<CKeyboard::EnKeyButton> keyboardButtontable;
			};

			/**
			 * @brief �A�N�V�Y�}�b�s���O��1�p�[�c
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