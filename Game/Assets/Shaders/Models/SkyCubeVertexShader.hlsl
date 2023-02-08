#include "SkyCubeHeader.hlsli"



SPSInput VSMain(SVSInput input)
{
	SPSInput output =
	{
		{0.0f,0.0f,0.0f,0.0f},	// svpos
		{0.0f,0.0f,0.0f}		// normal
	};

	output.svpos = mul(g_mWorld, input.pos);
	output.svpos = mul(g_mViewProj, output.svpos);

	float3x3 mWorldLocal3x3 = (float3x3)g_mWorld;
	//output.normal = normalize(mul(mWorldLocal3x3, input.normal));
	// �@���������ĉ�]�����Ȃ����Ƃɂ���āA�L���[�u�}�b�v����]�����邱�Ƃ��ł���B
	output.normal = normalize(input.normal);


	return output;
}