
/*!
 * @brief ���_�V�F�[�_�[�֓����Ă���f�[�^
 */
struct SVSInput
{
	float3 pos : POSITION;
	float3 color : COLOR;
};

/*!
 * @brief ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
 */
struct SPSInput
{
	float4 svpos : SV_POSITION;
	float3 color : COLOR;
};

cbuffer SceneDataCB : register(b0)
{
	float4x4 g_mViewProj;
}
