//// �e��錾 ////

// BasicModelHeader.hlsli�Ő錾����SVSInput�Ɠ����\���ɂ���
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


struct SPSInput
{
	float4 svpos : SV_POSITION;
	float3 normal : NORMAL;
};



//// �e�탌�W�X�^ ////

// BasicModelHeader.hlsli��

SamplerState g_sampler : register(s0);

// �X�L�j���O�p�̃{�[���s��
//StructuredBuffer<float4x4> g_mBones : register(t0);
// �C���X�^���V���O�p�̃��[���h�s��̔z��
//StructuredBuffer<float4x4> g_worldMatrixArray : register(t1);
// �e�N�X�`��
//Texture2D<float4> g_shadowMap : register(t2);
//Texture2D<float4> g_diffuseTexture : register(t3);
//Texture2D<float4> g_normalTexture : register(t4);

// g_diffuseTexture�̃��W�X�^�̂Ƃ���ɃL���[�u�}�b�v�����
TextureCube<float4> g_skyCubeMap : register(t3);


cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mViewProj;
	float4x4 g_mLightViewProj;
	float3 g_lightPos;
	int g_isShadowReceiver;
}

cbuffer ExpandCB : register(b1)
{
	float g_luminance;
}