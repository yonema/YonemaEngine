
// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct SOutput
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float3 normal : NORMAL0;	// 法線ベクトル
	float3 normalVS : NORMAL1;	// ビュー変換後の法線ベクトル
	float2 uv : TEXCOORD;		// uv
	float3 cameraToPosDirWS : VECTOR1;
	float3 cameraToPosDirVS : VECTOR2;
};

Texture2D<float4> g_texture : register(t0);	// 0番スロットに設定されたテクスチャ
Texture2D<float4> g_sphTexture : register(t1);
Texture2D<float4> g_spaTexture : register(t2);
SamplerState g_sampler : register(s0);	// 0番スロットに設定されたサンプラ

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix g_mWorldViewProj;
	matrix g_mWorld;
	matrix g_mView;
	matrix g_mProj;
	float3 g_cameraPosWS;
}

cbuffer Material : register(b1)
{
	float4 g_diffuse;
	float4 g_specular;
	float3 g_ambient;
}