#include "PeraPolygonHeader.hlsli"
#include "SimplePostEffectHeader.hlsli"


float4 PSMain(SPSIn psInput) : SV_TARGET
{
	float4 col = g_texture.Sample(g_sampler, psInput.uv);
	//return col;
	float Y = Grayscaling(col.rgb);
	//return float4(Y, Y, Y,1.0f);

	float3 invCol = InvertColor(col.rgb);
	//return float4(invCol, 1.0f);

	float3 posterizationCol = Posterization(col.rgb);
	//return float4(posterizationCol, 1.0f);

	float4 simpleBlurCol = SimpleBlur(psInput.uv);
	//return simpleBlurCol;

	float4 embossCol = Embossing(psInput.uv);
	//return embossCol;
	Y = Grayscaling(embossCol.rgb);
	//return float4(Y, Y, Y,1.0f);

	float4 sharpCol = Sharpness(psInput.uv);
	//return sharpCol;
	return col;


}