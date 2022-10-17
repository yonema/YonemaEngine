#include "BasicShaderHeader.hlsli"

SPSInput BasicVS(SVSInput svInput)
{
	SPSInput psInput;
	float w = svInput.weight / 100.0f;
	float4x4 bm = g_mBones[svInput.boneNo[0]] * w + g_mBones[svInput.boneNo[1]] * (1.0f - w);
	// �V�F�[�_�\�ł̍s�񉉎Z�͗�D��̂��߁A�������ɂ�����B
	// ���W�ɑ΂��āA�����ɍs���u���B
	psInput.svpos = mul(bm, svInput.pos);
	psInput.svpos = mul(g_mWorldViewProj, psInput.svpos);
	//psInput.svpos = mul(g_mWorldViewProj, svInput.pos);

	// �@���̕��s�ړ������𖳌��ɂ���B
	svInput.normal.w = 0.0f;
	psInput.normal = normalize(mul(g_mWorld, svInput.normal).xyz);
	psInput.normalVS = normalize(mul(g_mView, float4(psInput.normal,0.0f)).xyz);
	psInput.uv = svInput.uv;

	psInput.cameraToPosDirWS = normalize(svInput.pos.xyz - g_cameraPosWS);
	psInput.cameraToPosDirVS = normalize(mul(g_mView, float4(psInput.cameraToPosDirWS,0.0f)).xyz);

	return psInput;
}