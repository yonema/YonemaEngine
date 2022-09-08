#include "BasicShaderHeader.hlsli"

SOutput BasicVS(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneNo : BONE_NO,
	min16uint weight : WEIGHT,
	min16uint edge_flg : EDGE_FLG
)
{
	SOutput output;
	// シェーダ―での行列演算は列優先のため、左方向にかける。
	// 座標に対して、左側に行列を置く。
	output.svpos = mul(g_mat, pos);
	output.uv = uv;

	return output; 
}