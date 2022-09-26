#include "BasicShaderHeader.hlsli"

SOutput BasicVS(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	min16uint2 boneNo : BONE_NO,
	min16uint weight : WEIGHT,
	min16uint edge_flg : EDGE_FLG
)
{
	SOutput output;
	// �V�F�[�_�\�ł̍s�񉉎Z�͗�D��̂��߁A�������ɂ�����B
	// ���W�ɑ΂��āA�����ɍs���u���B
	output.svpos = mul(g_mWorldViewProj, pos);

	// �@���̕��s�ړ������𖳌��ɂ���B
	normal.w = 0.0f;
	output.normal = normalize(mul(g_mWorld, normal).xyz);
	output.normalVS = normalize(mul(g_mView, float4(output.normal,0.0f)).xyz);
	output.uv = uv;

	output.cameraToPosDirWS = normalize(pos.xyz - g_cameraPosWS);
	output.cameraToPosDirVS = normalize(mul(g_mView, output.cameraToPosDirWS));

	return output; 
}