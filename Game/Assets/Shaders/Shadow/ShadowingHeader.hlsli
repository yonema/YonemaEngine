
// #include "ShadowConstDataHeader.hlsli"

/**
 * @brief チェビシェフの不等式のコア関数
*/
float ChebyshevCore(in float2 moments, in float depth)
{
    // 遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
    float depth_sq = moments.x * moments.x;
    // このグループの分散具合を求める
    // 分散が大きいほど、varianceの数値は大きくなる
    float variance = moments.y - depth_sq;
    // このピクセルのライトから見た深度値とシャドウマップの平均の深度値の差を求める
    float md = depth - moments.x;
    // 光が届く確率を求める
    float lit_factor = variance / (variance + md * md);
    float lig_factor_min = 0.3f;
    // 光が届く確率の下限以下は影になるようにする。
    lit_factor = saturate((lit_factor - lig_factor_min) / (1.0f - lig_factor_min));
    // 光が届く確率から影になる確率を求める。
    return 1.0f - lit_factor;
}

/**
 * @brief チェビシェフの不等式を利用して、影になる可能性を計算する。
*/
float Chebyshev(in float2 moments, in float depth)
{
    // 早期リターンすると警告が出るため、関数を2つに分けて対応。

    float shadowRate = 0.0f;

    if (depth > moments.x)
    {
        shadowRate = ChebyshevCore(moments, depth);
    }


    return shadowRate;
}


float2 CalcShadowMapUVFromPosInLVP(in float4 posInLVP)
{
    // ライトビュープロジェクションでの座標を正規化して(-1.0f～1.0f)にする
    float2 shadowMapUV = posInLVP.xy / posInLVP.w;
    // 正規化スクリーン座標系(-1.0f～1.0f)から(-0.5f～0.5f)に変換
    // Y座標は反転させる
    shadowMapUV *= float2(0.5f, -0.5f);
    // (-0.5f～0.5f)からUV座標系(0.0f～1.0f)に変換
    shadowMapUV += 0.5f;

    return shadowMapUV;
}

/**
 * @brief 影になる可能性の計算のコア関数
*/
float CalcShadowRateCore(
    in Texture2D<float4> shadowMap,
    in float2 shadowMapUV,
    float zInLVP,
    in bool isSoftShadow
)
{
    float shadowRate = 0.0f;

    float4 shadowValue = shadowMap.Sample(g_sampler, shadowMapUV);

    if (shadowValue.x > 0.00000001f)
    {
#ifdef EVSM
        //zInLVP = exp(g_kExp * zInLVP);
        zInLVP = exp(g_kExp * (zInLVP - g_kDepthBias));
#else
        zInLVP -= g_kDepthBias;
#endif  // EVSM

        if (isSoftShadow)
        {
            // ソフトシャドウ。
            shadowRate = Chebyshev(shadowValue.xy, zInLVP);
        }
        else if (zInLVP >= shadowValue.r)
        {
            // ハードシャドウ。
            shadowRate = 1.0f;
        }
    }

    return shadowRate;
}

/**
 * @brief 影になる可能性を計算する。
*/
float CalcShadowRate(
    in Texture2D<float4> shadowMap,
    in float4 posInLVP,
    in float zInLVP,
    in bool isSoftShadow
)
{
    // 早期リターンすると警告が出るため、関数を2つに分けて対応。

    float shadowRate = 0.0f;

    float2 shadowMapUV = CalcShadowMapUVFromPosInLVP(posInLVP);

    // シャドウマップUVとライトビュープロジェクションの深度から、
    // シャドウマップの範囲内か調べる。
    if (shadowMapUV.x >= 0.0f && shadowMapUV.x <= 1.0f &&
        shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f &&
        zInLVP >= 0.0f && zInLVP <= 1.0f)
    {
        // シャドウマップの範囲内
        shadowRate = CalcShadowRateCore(shadowMap, shadowMapUV, zInLVP, isSoftShadow);

        // シャドウマップの端の方は、徐々に影を薄くする。
        if (shadowRate > 0.0000001f)
        {
            float fromCenterDist = length(shadowMapUV - float2(0.5f, 0.5f));
            // 正規化
            fromCenterDist /= g_k1DivRoot2;
            float distBias = saturate(1.0f - pow(fromCenterDist, g_kDistBiasPower));
            shadowRate *= distBias;
        }
    }

    return shadowRate;
}