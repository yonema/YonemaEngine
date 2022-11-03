#include "FbxTestHeader.hlsli"

float4 PSMain(SPSInput input) : SV_TARGET
{
	//float3 baseCol = input.color;
	//if (baseCol.r <= 0.01f && baseCol.g <= 0.01f && baseCol.b <= 0.01f)
	//{
	//	baseCol = float3(1.0f, 0.0f, 0.0f);
	//}
	float3 baseCol = g_diffuseTexture.Sample(g_sampler, input.uv).xyz;;
	float3 col = baseCol;
	float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
	float diffuse = dot(-lightDir, input.normal);

	col *= diffuse;

	col += baseCol * 0.2f;
	//col += g_ambient;

	return float4(col, 1.0f);
}