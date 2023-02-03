#include "ShadowMapHeader.hlsli"
#include "ShadowConstDataHeader.hlsli"

float4 PSMain(SPSInput input) : SV_TARGET
{
	//float depth = input.svpos.z;
	float depth = min(1.0f, length(input.worldPos.xyz - g_lightPos) / g_kMaxShadowDepth);

#ifdef EVSM
	float expDepth = exp(g_kExp * depth);
	return float4(expDepth, expDepth * expDepth, 0.0f, 1.0f);
#else
	return float4(depth, depth * depth, 0.0f, 1.0f);
#endif	// EVSM

}