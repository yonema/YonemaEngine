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
	// �V�F�[�_�\�ł̍s�񉉎Z�͗�D��̂��߁A�������ɂ�����B
	// ���W�ɑ΂��āA�����ɍs���u���B
	output.svpos = mul(g_mat, pos);
	output.uv = uv;

	return output; 
}