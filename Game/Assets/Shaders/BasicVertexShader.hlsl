#include "BasicHeader.hlsli"

SPSInput BasicVS(SVSInput vsInput)
{
	SPSInput psInput;
	float w = vsInput.weight / 100.0f;
	float4x4 bm = g_mBones[vsInput.boneNo[0]] * w + g_mBones[vsInput.boneNo[1]] * (1.0f - w);
	// �V�F�[�_�\�ł̍s�񉉎Z�͗�D��̂��߁A�������ɂ�����B
	// ���W�ɑ΂��āA�����ɍs���u���B
	psInput.svpos = mul(bm, vsInput.pos);
	psInput.svpos = mul(g_mWorldViewProj, psInput.svpos);
	//psInput.svpos = mul(g_mWorldViewProj, vsInput.pos);

	// �@���̕��s�ړ������𖳌��ɂ���B
	vsInput.normal.w = 0.0f;
	psInput.normal = normalize(mul(g_mWorld, vsInput.normal).xyz);
	psInput.normalVS = normalize(mul(g_mView, float4(psInput.normal,0.0f)).xyz);
	psInput.uv = vsInput.uv;

	psInput.cameraToPosDirWS = normalize(vsInput.pos.xyz - g_cameraPosWS);
	psInput.cameraToPosDirVS = normalize(mul(g_mView, float4(psInput.cameraToPosDirWS,0.0f)).xyz);

	return psInput;
}