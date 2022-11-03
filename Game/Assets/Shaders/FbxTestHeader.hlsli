

struct SVSInput
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SPSInput
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float3 normal : NORMAL0;	// �@���x�N�g��
	float4 color : COLOR;
	float2 uv : TEXCOORD;		// uv
};



SamplerState g_sampler : register(s0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��
Texture2D<float4> g_diffuseTexture : register(t0);


cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
	//float4x4 g_mBones[256];
}

cbuffer MaterialCB : register(b1)
{
	float4 g_ambient;
	float4 g_diffuse;
	float4 g_specular;
	float g_alpha;
}

