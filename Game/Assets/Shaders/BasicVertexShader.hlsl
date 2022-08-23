#include "BasicShaderHeader.hlsli"

SOutput BasicVS( float4 pos : POSITION, float2 uv : TEXCOORD )
{
	SOutput output;
	// シェーダ―での行列演算は列優先のため、左方向にかける。
	// 座標に対して、左側に行列を置く。
	output.svpos = mul(g_mat, pos);
	output.uv = uv;

	return output; 
}