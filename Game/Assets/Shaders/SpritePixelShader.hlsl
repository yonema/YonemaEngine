#include "SpriteHeader.hlsli"

float4 PSMain(SPSIn psInput) : SV_TARGET
{
	float4 col = g_texture.Sample(g_sampler, psInput.uv);
	col *= g_mulColor;

	//col.xyz = pow(max(col.xyz, 0.0001f), 1.0f / 2.2f);

	return col;
}