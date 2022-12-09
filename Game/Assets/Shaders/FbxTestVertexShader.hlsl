#include "FbxTestHeader.hlsli"

SPSInput VSMain(SVSInput input)
{
	SPSInput output;

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

	output.svpos = mul(mBone, input.pos);
	output.svpos = mul(g_mWorldViewProj, output.svpos);

	output.normal = normalize(mul(mBone, float4(input.normal, 0.0f)).xyz);
	output.normal = normalize(mul(g_mWorld, float4(output.normal, 0.0f)).xyz);

	output.color = input.color;

	output.uv = input.uv;

	return output;
}