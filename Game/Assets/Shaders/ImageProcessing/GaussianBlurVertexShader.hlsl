#include "GaussianBlurHeader.hlsli"

// 0～(g_kMaxBlurLevel -1)
static const int g_blurLevel = 2;
static const int g_kMaxBlurLevel = 4;
static const int g_kNumOffsets = 8;
static float g_kOffsetArrayByLevel[g_kMaxBlurLevel][g_kNumOffsets] =
{
    { 1.0f,3.0f,5.0f,7.0f,9.0f,11.0f,13.0f,15.0f },
    { 0.5f,1.0f,1.5f,2.0f,2.5f,3.0f,3.5f,4.0f },
    { 0.2f,0.4f,0.6f,0.8f,1.0f,1.2f,1.4f,1.6f },
    { 0.1f,0.2f,0.3f,0.4f,0.5f,0.6f,0.7f,0.8f }
};
static float g_kOffsetArray[g_kNumOffsets] = g_kOffsetArrayByLevel[g_blurLevel];

/*!
 * @brief Xブラー頂点シェーダー
 */
SPSInForBlur VSXBlur(SVSIn input)
{
    // step-13 横ブラー用の頂点シェーダーを実装
    SPSInForBlur output;

    // 座標変換
    output.pos = mul(g_mWorldViewProj, input.pos);

    // テクスチャサイズを取得
    float2 texSize;
    float level;
    g_texture.GetDimensions(0, texSize.x, texSize.y, level);

    // 基準テクセルのUV座標を記録
    float2 tex = input.uv;

    // 基準テクセルからU座標を+1テクセルずらすためのオフセットを計算する
    output.tex0.xy = float2(g_kOffsetArray[0] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+3テクセルずらすためのオフセットを計算する
    output.tex1.xy = float2(g_kOffsetArray[1] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+5テクセルずらすためのオフセットを計算する
    output.tex2.xy = float2(g_kOffsetArray[2] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+7テクセルずらすためのオフセットを計算する
    output.tex3.xy = float2(g_kOffsetArray[3] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+9テクセルずらすためのオフセットを計算する
    output.tex4.xy = float2(g_kOffsetArray[4] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+11テクセルずらすためのオフセットを計算する
    output.tex5.xy = float2(g_kOffsetArray[5] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+13テクセルずらすためのオフセットを計算する
    output.tex6.xy = float2(g_kOffsetArray[6] / texSize.x, 0.0f);

    // 基準テクセルからU座標を+15テクセルずらすためのオフセットを計算する
    output.tex7.xy = float2(g_kOffsetArray[7] / texSize.x, 0.0f);

    // オフセットに-1を掛けてマイナス方向のオフセットも計算する
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;

    // オフセットに基準テクセルのUV座標を足し算して、
    // 実際にサンプリングするUV座標に変換する
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);

    return output;
}



/*!
 * @brief Yブラー頂点シェーダー
 */
SPSInForBlur VSYBlur(SVSIn input)
{
    // step-14 Yブラー用の頂点シェーダーを実装

    SPSInForBlur output;

    // 座標変換
    output.pos = mul(g_mWorldViewProj, input.pos);

    // テクスチャサイズを取得
    float2 texSize;
    float level;
    g_texture.GetDimensions(0, texSize.x, texSize.y, level);

    // 基準テクセルのUV座標を記録
    float2 tex = input.uv;

    // 基準テクセルからV座標を+1テクセルずらすためのオフセットを計算する
    output.tex0.xy = float2(0.0f, g_kOffsetArray[0] / texSize.y);

    // 基準テクセルからV座標を+3テクセルずらすためのオフセットを計算する
    output.tex1.xy = float2(0.0f, g_kOffsetArray[1] / texSize.y);

    // 基準テクセルからV座標を+5テクセルずらすためのオフセットを計算する
    output.tex2.xy = float2(0.0f, g_kOffsetArray[2] / texSize.y);

    // 基準テクセルからV座標を+7テクセルずらすためのオフセットを計算する
    output.tex3.xy = float2(0.0f, g_kOffsetArray[3] / texSize.y);

    // 基準テクセルからV座標を+9テクセルずらすためのオフセットを計算する
    output.tex4.xy = float2(0.0f, g_kOffsetArray[4] / texSize.y);

    // 基準テクセルからV座標を+11テクセルずらすためのオフセットを計算する
    output.tex5.xy = float2(0.0f, g_kOffsetArray[5] / texSize.y);

    // 基準テクセルからV座標を+13テクセルずらすためのオフセットを計算する
    output.tex6.xy = float2(0.0f, g_kOffsetArray[6] / texSize.y);

    // 基準テクセルからV座標を+15テクセルずらすためのオフセットを計算する
    output.tex7.xy = float2(0.0f, g_kOffsetArray[7] / texSize.y);

    // オフセットに-1を掛けてマイナス方向のオフセットも計算する
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;

    // オフセットに基準テクセルのUV座標を足し算して、
    // 実際にサンプリングするUV座標に変換する
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);

    return output;
}