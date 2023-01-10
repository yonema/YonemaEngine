#include "BasicModelHeader.hlsli"

float4 PSMain(SPSInput input) : SV_TARGET
{
	float4 diffuseTexCol = g_diffuseTexture.Sample(g_sampler, input.uv);

	// alphaClip
	clip(diffuseTexCol.a <= 0.1f ? -1 : 1);

	float3 baseCol = diffuseTexCol.xyz;
	float3 col = baseCol;
	float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
	float diffuse = dot(-lightDir, input.normal.xyz);

	col = max(col * diffuse, baseCol * 0.2f);

	return float4(col, 1.0f);
}