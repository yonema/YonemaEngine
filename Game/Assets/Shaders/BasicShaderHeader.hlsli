
// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float2 uv : TEXCOORD;		// uv
};

Texture2D<float4> g_texture : register(t0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState g_sampler : register(s0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

// �萔�o�b�t�@
cbuffer cbuff0 : register(b0)
{
	matrix g_mat;
}