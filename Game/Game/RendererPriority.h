#pragma once
namespace nsYMEngine
{
	namespace nsGraphics
	{
		namespace nsRenderers
		{

			/**
			 * @brief レンダラーの優先度。
			 * 優先度によって更新順番が変化します。
			 * 優先度の値が低いゲームオブジェクトほど先に、
			 * 優先度の値が高いゲームオブジェクトほど後に、
			 * 更新処理が行われます。
			*/
			enum class EnRendererPriority
			{
				// 優先度の最低値。
				// これより低い値の優先度は作ってはいけません。
				enMin,



				// 優先度の中間値
				enMid,



				// 優先度の最大値
				// これより高い値の優先度を作ってはいけません。
				enMax,
				enNum
			};


			static constexpr unsigned int g_kNumRendererPriority = 
				static_cast<unsigned int>(EnRendererPriority::enNum);

		}
	}
}