#pragma once

namespace nsYMEngine
{
	namespace nsCommonDataLibrary
	{
		// ゲームオブジェクトの優先度の最高値
		static constexpr unsigned int g_kMaxGameObjectPriority = 99;
		// ゲームオブジェクトの優先度の中間地
		static constexpr unsigned int g_kMidGameObjectPriority = g_kMaxGameObjectPriority / 2;
		// ゲームオブジェクトの優先度の最低値
		static constexpr unsigned int g_kMinGameObjectPriority = 0;
	}

	/**
	 * @brief ゲームオブジェクトの優先度。
	 * 優先度によって更新順番が変化します。
	 * 優先度の値が低いゲームオブジェクトほど先に、
	 * 優先度の値が高いゲームオブジェクトほど後に、
	 * 更新処理が行われます。
	*/
	enum class EnGOPriority
	{
		// 優先度の最低値。
		// これより低い値の優先度は作ってはいけません。
		enMin = nsCommonDataLibrary::g_kMinGameObjectPriority,




		// 優先度の中間値
		enMid = nsCommonDataLibrary::g_kMidGameObjectPriority,





		// 優先度の最大値
		// これより高い値の優先度を作ってはいけません。
		enMax = nsCommonDataLibrary::g_kMaxGameObjectPriority
	};

	/**
	 * @brief スプライトやフォントで使用するアンカー
	*/
	enum class EnAnchors
	{
		enTopLeft,			// 左上
		enTopCenter,		// 中央上
		enTopRight,			// 右上
		enMiddleLeft,		// 左中央
		enMiddleCenter,		// 中央
		enMiddleRight,		// 右中央
		enBottonLeft,		// 左下
		enBottonCenter,		// 中央下
		enBottonRight		// 中央右
	};


	enum class EnLoadingState
	{
		enBeforeLoading,
		enNowLoading,
		enAfterLoading
	};



}