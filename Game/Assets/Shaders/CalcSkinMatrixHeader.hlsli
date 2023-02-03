

/**
 * @brief スキン行列を計算する
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

	// ボーンウェイトを考慮して、ボーン行列を計算
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float weight = input.weight[i];
		totalWeight += input.weight[i];
		weight /= 10000.0f;
		mBone += g_mBones[input.boneNo[i]] * weight;
	}

	// ボーンウェイトが全て0だったときは、単位行列を入れる
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