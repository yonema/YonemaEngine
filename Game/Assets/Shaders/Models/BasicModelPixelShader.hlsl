#include "BasicModelHeader.hlsli"
#include "../Shadow/ShadowConstDataHeader.hlsli"
#include "../Shadow/ShadowingHeader.hlsli"

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
	float3 finalCol = baseCol;

	float3 lightDir = normalize(float3(1.0f, -1.0f, 1.0f));
	float diffuse = dot(-lightDir, normalWS);

	float3 shadowCol = baseCol * 0.2f;
	float shadowRate = 0.0f;
	if (g_isShadowReceiver)
	{
		float zInLVP = min(1.0f, length(input.posInWorld.xyz - g_lightPos) / g_kMaxShadowDepth);
		shadowRate = CalcShadowRate(g_shadowMap, input.posInLVP, zInLVP, true);
	}

	finalCol = max(finalCol * diffuse, baseCol * 0.2f);
	finalCol = lerp(finalCol, shadowCol, shadowRate);

	return float4(finalCol, 1.0f);
}