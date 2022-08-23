#include "BasicShaderHeader.hlsli"

SOutput BasicVS( float4 pos : POSITION, float2 uv : TEXCOORD )
{
	SOutput output;
	// �V�F�[�_�\�ł̍s�񉉎Z�͗�D��̂��߁A�������ɂ�����B
	// ���W�ɑ΂��āA�����ɍs���u���B
	output.svpos = mul(g_mat, pos);
	output.uv = uv;

	return output; 
}