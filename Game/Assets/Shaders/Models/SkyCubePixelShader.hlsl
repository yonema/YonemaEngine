#include "SkyCubeHeader.hlsli"


float4 PSMain(SPSInput input) : SV_TARGET
{
	//float4 color = g_skyCubeMap.Sample(g_sampler, input.normal * -1.0f) * luminance;
	float4 color = g_skyCubeMap.Sample(g_sampler, input.normal * -1.0f) * g_luminance;
	return color;
	//return float4(1.0f,0.0f,0.0f,1.0f);
}