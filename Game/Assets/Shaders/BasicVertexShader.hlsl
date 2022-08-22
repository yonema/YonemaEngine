#include "BasicShaderHeader.hlsli"

SOutput BasicVS( float4 pos : POSITION, float2 uv : TEXCOORD )
{
	SOutput output;
	output.svpos = pos;
	output.uv = uv;

	return output; 
}