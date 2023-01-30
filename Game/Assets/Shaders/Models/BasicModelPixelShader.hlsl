#include "BasicModelHeader.hlsli"

float4 PSMain(SPSInput input) : SV_TARGET
{
	float4 diffuseTexCol = g_diffuseTexture.Sample(g_sampler, input.uv);

	// alphaClip
	clip(diffuseTexCol.a <= 0.5f ? -1 : 1);

	// CalcNormalWSFromNormalTS
	float3 normalTS = g_normalTexture.Sample(g_sampler, input.uv).xyz;
	// –@ü‚Ì’l‚Ì”ÍˆÍ‚ðA0.0f`1.0f‚©‚ç-1.0f`1.0f‚É•œŒ³‚·‚éB
	normalTS = (normalTS - 0.5f) * 2.0f;
	float3 normalWS = 
		input.tangent * normalTS.x + input.biNormal * normalTS.y + input.normal * normalTS.z;
	//normalWS = normalize(normalWS);
	



	float3 baseCol = diffuseTexCol.xyz;
	float3 col = baseCol;
	float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
	float diffuse = dot(-lightDir, normalWS);

	col = max(col * diffuse, baseCol * 0.2f);

	return float4(col, 1.0f);
}