#include "FbxTestHeader.hlsli"

SPSInput VSMain(SVSInput input)
{
	SPSInput output;

	float4x4 mBone = 0;
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float weight = input.weight[i];
		weight /= 10000.0f;
		mBone += g_mBones[input.boneNo[i]] * weight;
	}

	output.svpos = mul(mBone, input.pos);
	output.svpos = mul(g_mWorldViewProj, output.svpos);
	output.svpos = mul(g_mWorldViewProj, input.pos);

	output.normal = normalize(mul(g_mWorld, float4(input.normal, 0.0f)).xyz);

	output.color = input.color;

	output.uv = input.uv;

	return output;
}