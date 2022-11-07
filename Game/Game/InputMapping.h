#pragma once
#include "../YonemaEngine/Input/InputManagerData.h"



/**
* @file InputMapping.h
* @brief 仮想ボタンを作成し、ゲームパッドやキーボードに割り当てるためのマッピングデータのファイル
* @details EnActionMappingやEnAxisMappingで、入力の種類を増やします。
* g_kActionMappingTableとg_kAxisMappingTableで増やした分の入力に、ボタンを割り当てます。
*/


namespace nsYMEngine
{
	namespace nsInput
	{
		/**
		 * @brief アクションマッピング
		 * ボタンのTriggerやPressが呼べるようになる入力マッピング
		*/
		enum class EnActionMapping
		{
			enAction,
			enJump,
			enNumActions
		};

		/**
		 * @brief アクシズマッピング
		 * 軸入力値をがとれるようになる入力マッピング
		*/
		enum class EnAxisMapping
		{
			enForward,
			enRight,
			enNumAxes
		};

		/**
		 * @brief アクションマッピングの入力内容の設定テーブル
		*/
		const nsInputManagerData::SActionMapChip
			g_kActionMappingTable[static_cast<int>(EnActionMapping::enNumActions)] =
		{
			//// enExsample	// アクションマッピング
			//{
			//	// ゲームパッドのボタンの割り当て
			//	// {割り当てるゲームパッドの仮想ボタン1, 割り当てるゲームパッドの仮想ボタン2 ...}
			//	{CGamepad::EnPadButton::enA ,CGamepad::EnPadButton::enB},
			//	// キーボードのボタンの割り当て
			//	// {割り当てるキーボードの仮想ボタン1, 割り当てるキーボードの仮想ボタン2 ...}
			//	{CKeyboard::EnKeyButton::enEnter,CKeyboard::EnKeyButton::enSpace }
			//},


			// enAction
			{
				{CGamepad::EnPadButton::enB ,CGamepad::EnPadButton::enX,CGamepad::EnPadButton::enY,},
				{CKeyboard::EnKeyButton::enEnter}
			},
			// enJump
			{
				{CGamepad::EnPadButton::enA},
				{CKeyboard::EnKeyButton::enSpace,CKeyboard::EnKeyButton::enLShift}
			}
		};

		/**
		 * @brief アクシズマッピングの入力内容の設定テーブル
		*/
		const nsInputManagerData::SAxisMapChip
			g_kAxisMappingTable[static_cast<int>(EnAxisMapping::enNumAxes)] =
		{
			//// enExsample	// アクシズマッピング
			//{
			//	// ゲームパッドのボタンの割り当て
			//	// {{割り当てるゲームパッドの仮想ボタン1, 軸入力の値1},{割り当てるゲームパッドの仮想ボタン2, 軸入力の値2}}
			//	{{CGamepad::EnPadButton::enUp, 1.0f}, {CGamepad::EnPadButton::enDown, -1.0f}},
			//	// キーボードのボタンの割り当て
			//	// {{割り当てるキーボードの仮想ボタン1, 軸入力の値1},{割り当てるキーボードの仮想ボタン2, 軸入力の値2}}
			//	{{CKeyboard::EnKeyButton::enW, 1.0f}, {CKeyboard::EnKeyButton::enS, -1.0f}},
			//	// ゲームパッドの軸入力の割り当て
			//	// {{割り当てるゲームパッドの仮想軸入力1, 軸入力のスケール1},{割り当てるゲームパッドの仮想軸入力2, 軸入力のスケール2}}
			//	{{nsInputManagerData::EnGamepadStickAxis::enLStickY, 1.0f}, {nsInputManagerData::EnGamepadStickAxis::enRStickY, -1.0f}}
			//},
			

			// enForward
			{
				{{CGamepad::EnPadButton::enUp, 1.0f}, {CGamepad::EnPadButton::enDown, -1.0f}},
				{{CKeyboard::EnKeyButton::enW, 1.0f}, {CKeyboard::EnKeyButton::enS, -1.0f}},
				{{nsInputManagerData::EnGamepadStickAxis::enLStickY, 1.0f}, {nsInputManagerData::EnGamepadStickAxis::enRStickY, -1.0f}}
			},
			// enRight
			{
				{{CGamepad::EnPadButton::enLeft, -1.0f}, {CGamepad::EnPadButton::enRight, 1.0f}},
				{{CKeyboard::EnKeyButton::enA, -1.0f},{CKeyboard::EnKeyButton::enD, 1.0f}},
				{{nsInputManagerData::EnGamepadStickAxis::enLStickX, 1.0f}, {nsInputManagerData::EnGamepadStickAxis::enRStickX, -1.0f}}
			}
		};


	}
}
