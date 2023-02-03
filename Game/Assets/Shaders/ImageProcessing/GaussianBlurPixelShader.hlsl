#include "GaussianBlurHeader.hlsli"


/*!
 * @brief ブラーピクセルシェーダー
 */
float4 PSBlur(SPSInForBlur input) : SV_Target0
{
    // step-15 X,Yブラー用のピクセルシェーダーを実装
    float4 Color;

    // 基準テクセルからプラス方向に8テクセル、重み付きでサンプリング
    Color = g_weight[0].x * g_texture.Sample(g_sampler, input.tex0.xy);
    Color += g_weight[0].y * g_texture.Sample(g_sampler, input.tex1.xy);
    Color += g_weight[0].z * g_texture.Sample(g_sampler, input.tex2.xy);
    Color += g_weight[0].w * g_texture.Sample(g_sampler, input.tex3.xy);
    Color += g_weight[1].x * g_texture.Sample(g_sampler, input.tex4.xy);
    Color += g_weight[1].y * g_texture.Sample(g_sampler, input.tex5.xy);
    Color += g_weight[1].z * g_texture.Sample(g_sampler, input.tex6.xy);
    Color += g_weight[1].w * g_texture.Sample(g_sampler, input.tex7.xy);
    
    // 基準テクセルにマイナス方向に8テクセル、重み付きでサンプリング
    Color += g_weight[0].x * g_texture.Sample(g_sampler, input.tex0.zw);
    Color += g_weight[0].y * g_texture.Sample(g_sampler, input.tex1.zw);
    Color += g_weight[0].z * g_texture.Sample(g_sampler, input.tex2.zw);
    Color += g_weight[0].w * g_texture.Sample(g_sampler, input.tex3.zw);
    Color += g_weight[1].x * g_texture.Sample(g_sampler, input.tex4.zw);
    Color += g_weight[1].y * g_texture.Sample(g_sampler, input.tex5.zw);
    Color += g_weight[1].z * g_texture.Sample(g_sampler, input.tex6.zw);
    Color += g_weight[1].w * g_texture.Sample(g_sampler, input.tex7.zw);
    
    return float4(Color.xyz, 1.0f);
}