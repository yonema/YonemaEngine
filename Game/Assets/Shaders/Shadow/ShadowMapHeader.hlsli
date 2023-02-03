//// 各種宣言 ////

struct SVSInput
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float2 uv : TEXCOORD;
	min16uint4 boneNo : BONE_NO;
	min16uint4 weight : WEIGHT;
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct SPSInput
{
	float4 svpos : SV_POSITION;
	float4 worldPos : TEXCOORD;
};


//// 各種レジスタ ////

// スキニング用のボーン行列
StructuredBuffer<float4x4> g_mBones : register(t0);
// インスタンシング用のワールド行列の配列
StructuredBuffer<float4x4> g_worldMatrixArray : register(t1);

cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mViewProj;
	float3 g_lightPos;
}