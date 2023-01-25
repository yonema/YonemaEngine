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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float2 uv : TEXCOORD;
};


//// 各種レジスタ ////

SamplerState g_sampler : register(s0);

// スキニング用のボーン行列
StructuredBuffer<float4x4> g_mBones : register(t0);
// インスタンシング用のワールド行列の配列
StructuredBuffer<float4x4> g_worldMatrixArray : register(t1);
// テクスチャ
Texture2D<float4> g_diffuseTexture : register(t2);
Texture2D<float4> g_normalTexture : register(t3);

cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mViewProj;
}


//// 関数 ////

/**
 * @brief スキン行列を計算する
*/
float4x4 CalcSkinMatrix(SVSInput input)
{
	int totalWeight = 0;
	float4x4 mBone = {
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	};

	// ボーンウェイトを考慮して、ボーン行列を計算
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float weight = input.weight[i];
		totalWeight += input.weight[i];
		weight /= 10000.0f;
		mBone += g_mBones[input.boneNo[i]] * weight;
	}

	// ボーンウェイトが全て0だったときは、単位行列を入れる
	if (totalWeight == 0)
	{
		mBone = float4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}


	return mBone;
}