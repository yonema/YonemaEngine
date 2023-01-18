#include "BasicModelHeader.hlsli"



/**
 * @brief ���_�V�F�[�_�[�̃R�A����
*/
SPSInput VSMainCore(in SVSInput input, in float4x4 mWorldLocal)
{
	SPSInput output =
	{
		{0.0f,0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f}
	};

	output.svpos = mul(mWorldLocal, input.pos);
	output.svpos = mul(g_mViewProj, output.svpos);

	output.normal = normalize(mul(mWorldLocal, float4(input.normal, 0.0f)).xyz);

	output.uv = input.uv;

	return output;
}


//// ���_�V�F�[�_�[�̊e��G���g���[�|�C���g ////

SPSInput VSMain(SVSInput input)
{
	return VSMainCore(input, g_mWorld);
}

SPSInput VSMainForSkinning(SVSInput input)
{
	return VSMainCore(input, mul(g_mWorld, CalcSkinMatrix(input)));
}

SPSInput VSMainForInstancing(SVSInput input, uint instanceID : SV_InstanceID)
{
	return VSMainCore(input, mul(g_worldMatrixArray[instanceID], g_mWorld));
}