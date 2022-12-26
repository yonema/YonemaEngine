#pragma once
namespace nsYMEngine
{
	namespace nsSound
	{
		/**
		 * @brief サウンドのサブミックスの種類。
		 * サブミックスタイプごとに、まとめて音量を設定したり、エフェクトをかけたりできます。
		*/
		enum class EnSubmixType
		{
			enSE,
			enBGM,
			// ユーザー定義、ここらか


			// ユーザー定義、ここまで
			enNum
		};
	}
}