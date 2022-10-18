
/**
 * @brief rgbをグレースケール化する。PALの規格で定義されている変換式を使用。
 * @param rgb 元になるrgb色
 * @return グレースケール値
*/
float Grayscaling(float3 rgb)
{
	return dot(rgb, float3(0.299, 0.587, 0.114));
}

/**
 * @brief 色調反転。色を反転する。
 * @param col 元になる色
 * @return 反転した色
*/
float3 InvertColor(float3 col)
{
	return 1.0f - col;
}

/**
 * @brief ポスタリゼーション。色の階調を落とす。
 * @param col 元になる色
 * @return 階調を落とした色
*/
float3 Posterization(float3 col)
{
	return col - fmod(col.rgb, 0.25f);
}

/**
 * @brief 単純なブラー処理。あまりきれいなぼかしじゃない。
 * テクスチャにg_texture、サンプラーにg_samplerが定義されていないと動かない。
 * @param uv 現在のピクセルのuv
 * @return ブラーがかかった色
*/
float4 SimpleBlur(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// 左上
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));				// 上
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));		// 右上
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));				// 左
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));				// 中央（自分）
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));				// 右
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));		// 左下
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));				// 下
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));		// 右下

	col /= 9.0f;

	return col;
}

/**
 * @brief エンボス加工。浮き彫り加工。
 * テクスチャにg_texture、サンプラーにg_samplerが定義されていないと動かない。
 * エンボス加工は色がついていると微妙な感じがするので、
 * エンボス加工後にグレースケール化するといい感じになる。
 * @param uv 現在のピクセルのuv
 * @return エンボス加工した色
*/
float4 Embossing(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	// 全体の合計値が1になるようにする。
	// 1になるため、平均する必要なし。
	// 例として、左上を浮き上がらせ、右下をへこませる処理を行う。

	col += 2.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// 左上
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));				// 上
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));		// 右上
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));				// 左
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));				// 中央（自分）
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));		// 右
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));	// 左下
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));		// 下
	col += -2.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));// 右下

	return col;
}

/**
 * @brief シャープネス。エッジの強調。
 * テクスチャにg_texture、サンプラーにg_samplerが定義されていないと動かない。
 * @param uv 現在のピクセルのuv
 * @return シャープネスをかけた色
*/
float4 Sharpness(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	// 全体の合計値が1になるようにする。
	// 1になるため、平均する必要なし。

	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// 左上
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));		// 上
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));// 右上
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));		// 左
	col += 5.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));			// 中央（自分）
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));		// 右
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));	// 左下
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));		// 下
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));// 右下

	return col;
}