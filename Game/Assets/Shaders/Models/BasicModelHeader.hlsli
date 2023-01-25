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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 biNormal : BINORMAL;
	float2 uv : TEXCOORD;
};


//// �e�탌�W�X�^ ////

SamplerState g_sampler : register(s0);

// �X�L�j���O�p�̃{�[���s��
StructuredBuffer<float4x4> g_mBones : register(t0);
// �C���X�^���V���O�p�̃��[���h�s��̔z��
StructuredBuffer<float4x4> g_worldMatrixArray : register(t1);
// �e�N�X�`��
Texture2D<float4> g_diffuseTexture : register(t2);
Texture2D<float4> g_normalTexture : register(t3);

cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mViewProj;
}


//// �֐� ////

/**
 * @brief �X�L���s����v�Z����
*/
float4x4 CalcSkinMatrix(SVSInput input)
{
	int totalWeight = 0;
	float4x4 mBone = {
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,0.0f
	};

	// �{�[���E�F�C�g���l�����āA�{�[���s����v�Z
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float weight = input.weight[i];
		totalWeight += input.weight[i];
		weight /= 10000.0f;
		mBone += g_mBones[input.boneNo[i]] * weight;
	}

	// �{�[���E�F�C�g���S��0�������Ƃ��́A�P�ʍs�������
	if (totalWeight == 0)
	{
		mBone = float4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}


	return mBone;
}