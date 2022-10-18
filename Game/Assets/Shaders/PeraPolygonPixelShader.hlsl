#include "PeraPolygonHeader.hlsli"

float4 PSMain(SPSIn psInput) : SV_TARGET
{
	float4 col = g_texture.Sample(g_sampler, psInput.uv);
	return col;
}