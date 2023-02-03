//// �e��錾 ////

struct SVSInput
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float2 uv : TEXCOORD;
	min16uint4 boneNo : BONE_NO;
	min16uint4 weight : WEIGHT;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SPSInput
{
	float4 svpos : SV_POSITION;
	float4 worldPos : TEXCOORD;
};


//// �e�탌�W�X�^ ////

// �X�L�j���O�p�̃{�[���s��
StructuredBuffer<float4x4> g_mBones : register(t0);
// �C���X�^���V���O�p�̃��[���h�s��̔z��
StructuredBuffer<float4x4> g_worldMatrixArray : register(t1);

cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mViewProj;
	float3 g_lightPos;
}