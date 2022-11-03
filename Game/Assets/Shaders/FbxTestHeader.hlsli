

struct SVSInput
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct SPSInput
{
	float4 svpos : SV_POSITION;	// システム用頂点座標
	float3 normal : NORMAL0;	// 法線ベクトル
	float4 color : COLOR;
	float2 uv : TEXCOORD;		// uv
};



SamplerState g_sampler : register(s0);	// 0番スロットに設定されたサンプラ
Texture2D<float4> g_diffuseTexture : register(t0);


cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
	//float4x4 g_mBones[256];
}

cbuffer MaterialCB : register(b1)
{
	float4 g_ambient;
	float4 g_diffuse;
	float4 g_specular;
	float g_alpha;
}

