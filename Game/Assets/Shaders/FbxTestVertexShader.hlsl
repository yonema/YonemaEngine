#include "FbxTestHeader.hlsli"

SPSInput VSMain(SVSInput input)
{
	SPSInput output;
	
	output.svpos = mul(g_mWorldViewProj, input.pos);

	output.normal = normalize(mul(g_mWorld, input.normal).xyz);

	output.color = input.color;

	output.uv = input.uv;

	return output;
}