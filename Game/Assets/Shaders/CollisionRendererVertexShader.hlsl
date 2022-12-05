#include "CollisionRendererHeader.hlsli"

SPSInput VSMain(SVSInput input)
{
	SPSInput output;
	output.svpos = mul(g_mViewProj, float4(input.pos, 1.0f));
	output.color = input.color;

	return output;
}