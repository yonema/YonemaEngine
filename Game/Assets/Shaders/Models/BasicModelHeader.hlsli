
struct SVSInput
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	min16uint4 boneNo : BONE_NO;
	min16uint4 weight : WEIGHT;
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct SPSInput
{
	float4 svpos : SV_POSITION;	// �V�X�e���p���_���W
	float3 normal : NORMAL0;	// �@���x�N�g��
	float4 color : COLOR;
	float2 uv : TEXCOORD;		// uv
};

struct SBasicModelCB
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
};

SamplerState g_sampler : register(s0);	// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��
Texture2D<float4> g_diffuseTexture : register(t0);


SPSInput VSCore(in SVSInput input, in float4x4 mWorld, in float4x4 mWorldViewProj)
{
	SPSInput output;

	output.svpos = mul(mWorldViewProj, input.pos);

	output.normal = normalize(mul(mWorld, float4(input.normal, 0.0f)).xyz);

	output.color = input.color;

	output.uv = input.uv;

	return output;
}