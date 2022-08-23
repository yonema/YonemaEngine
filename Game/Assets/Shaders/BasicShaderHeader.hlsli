
// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct SOutput
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float2 uv : TEXCOORD;		// uv
};

Texture2D<float4> g_texture : register(t0);	// 0番スロットに設定されたテクスチャ
SamplerState g_sampler : register(s0);	// 0番スロットに設定されたサンプラ

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix g_mat;
}