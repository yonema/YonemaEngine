#include "BasicShaderHeader.hlsli"

float4 BasicPS(SOutput input) : SV_TARGET
{
	return float4(g_texture.Sample(g_sampler, input.uv));
}