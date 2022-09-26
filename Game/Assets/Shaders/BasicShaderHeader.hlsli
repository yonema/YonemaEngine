
// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SOutput
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float3 normal : NORMAL0;	// �@���x�N�g��
	float3 normalVS : NORMAL1;	// �r���[�ϊ���̖@���x�N�g��
	float2 uv : TEXCOORD;		// uv
	float3 cameraToPosDirWS : VECTOR1;
	float3 cameraToPosDirVS : VECTOR2;
};

Texture2D<float4> g_texture : register(t0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> g_sphTexture : register(t1);
Texture2D<float4> g_spaTexture : register(t2);
SamplerState g_sampler : register(s0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

// �萔�o�b�t�@
cbuffer cbuff0 : register(b0)
{
	matrix g_mWorldViewProj;
	matrix g_mWorld;
	matrix g_mView;
	matrix g_mProj;
	float3 g_cameraPosWS;
}

cbuffer Material : register(b1)
{
	float4 g_diffuse;
	float4 g_specular;
	float3 g_ambient;
}