#include "BasicHeader.hlsli"

SPSInput BasicVS(SVSInput input)
{
	SPSInput output;
	float w = input.weight / 100.0f;
	float4x4 bm = g_mBones[input.boneNo[0]] * w + g_mBones[input.boneNo[1]] * (1.0f - w);
	// シェーダ―での行列演算は列優先のため、左方向にかける。
	// 座標に対して、左側に行列を置く。
	output.svpos = mul(bm, input.pos);
	output.svpos = mul(g_mWorldViewProj, output.svpos);
	//output.svpos = mul(g_mWorldViewProj, input.pos);

	// 法線の平行移動成分を無効にする。
	input.normal.w = 0.0f;
	output.normal = normalize(mul(g_mWorld, input.normal).xyz);
	output.normalVS = normalize(mul(g_mView, float4(output.normal,0.0f)).xyz);
	output.uv = input.uv;

	output.cameraToPosDirWS = normalize(input.pos.xyz - g_cameraPosWS);
	output.cameraToPosDirVS = normalize(mul(g_mView, float4(output.cameraToPosDirWS,0.0f)).xyz);

	return output;
}