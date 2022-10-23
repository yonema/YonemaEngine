

struct SVSInput
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD;
	min16uint2 boneNo : BONE_NO;
	min16uint weight : WEIGHT;
	min16uint edge_flg : EDGE_FLG;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SPSInput
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

cbuffer SceneDataCB : register(b0)
{
	float4x4 g_mView;
	float4x4 g_mProj;
	float3 g_cameraPosWS;
}

cbuffer ModelCB : register(b1) 
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
	float4x4 g_mBones[256];
}

cbuffer MaterialCB : register(b2)
{
	float4 g_diffuse;
	float4 g_specular;
	float3 g_ambient;
}