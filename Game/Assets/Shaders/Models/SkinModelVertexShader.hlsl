#include "BasicModelHeader.hlsli"

cbuffer SkinModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
	// 固定長配列のため、あらかじめ多めに256個のボーン行列を確保。
	float4x4 g_mBones[256];
}

SPSInput VSMain(SVSInput input)
{
	int totalWeight = 0;
	float4x4 mBone = {
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	};

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float weight = input.weight[i];
		totalWeight += input.weight[i];
		weight /= 10000.0f;
		mBone += g_mBones[input.boneNo[i]] * weight;
	}
	if (totalWeight == 0)
	{
		mBone = float4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	input.pos = mul(mBone, input.pos);
	input.normal = normalize(mul(mBone, float4(input.normal, 0.0f)).xyz);

	return VSCore(input, g_mWorld, g_mWorldViewProj);
}