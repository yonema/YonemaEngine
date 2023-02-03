

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