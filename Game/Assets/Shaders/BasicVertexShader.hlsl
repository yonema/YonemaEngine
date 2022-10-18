#include "BasicHeader.hlsli"

SPSInput BasicVS(SVSInput vsInput)
{
	SPSInput psInput;
	float w = vsInput.weight / 100.0f;
	float4x4 bm = g_mBones[vsInput.boneNo[0]] * w + g_mBones[vsInput.boneNo[1]] * (1.0f - w);
	// シェーダ―での行列演算は列優先のため、左方向にかける。
	// 座標に対して、左側に行列を置く。
	psInput.svpos = mul(bm, vsInput.pos);
	psInput.svpos = mul(g_mWorldViewProj, psInput.svpos);
	//psInput.svpos = mul(g_mWorldViewProj, vsInput.pos);

	// 法線の平行移動成分を無効にする。
	vsInput.normal.w = 0.0f;
	psInput.normal = normalize(mul(g_mWorld, vsInput.normal).xyz);
	psInput.normalVS = normalize(mul(g_mView, float4(psInput.normal,0.0f)).xyz);
	psInput.uv = vsInput.uv;

	psInput.cameraToPosDirWS = normalize(vsInput.pos.xyz - g_cameraPosWS);
	psInput.cameraToPosDirVS = normalize(mul(g_mView, float4(psInput.cameraToPosDirWS,0.0f)).xyz);

	return psInput;
}