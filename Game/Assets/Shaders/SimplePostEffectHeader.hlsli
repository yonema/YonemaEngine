
/**
 * @brief rgb���O���[�X�P�[��������BPAL�̋K�i�Œ�`����Ă���ϊ������g�p�B
 * @param rgb ���ɂȂ�rgb�F
 * @return �O���[�X�P�[���l
*/
float Grayscaling(float3 rgb)
{
	return dot(rgb, float3(0.299, 0.587, 0.114));
}

/**
 * @brief �F�����]�B�F�𔽓]����B
 * @param col ���ɂȂ�F
 * @return ���]�����F
*/
float3 InvertColor(float3 col)
{
	return 1.0f - col;
}

/**
 * @brief �|�X�^���[�[�V�����B�F�̊K���𗎂Ƃ��B
 * @param col ���ɂȂ�F
 * @return �K���𗎂Ƃ����F
*/
float3 Posterization(float3 col)
{
	return col - fmod(col.rgb, 0.25f);
}

/**
 * @brief �P���ȃu���[�����B���܂肫�ꂢ�Ȃڂ�������Ȃ��B
 * �e�N�X�`����g_texture�A�T���v���[��g_sampler����`����Ă��Ȃ��Ɠ����Ȃ��B
 * @param uv ���݂̃s�N�Z����uv
 * @return �u���[�����������F
*/
float4 SimpleBlur(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// ����
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));				// ��
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));		// �E��
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));				// ��
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));				// �����i�����j
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));				// �E
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));		// ����
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));				// ��
	col += g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));		// �E��

	col /= 9.0f;

	return col;
}

/**
 * @brief �G���{�X���H�B����������H�B
 * �e�N�X�`����g_texture�A�T���v���[��g_sampler����`����Ă��Ȃ��Ɠ����Ȃ��B
 * �G���{�X���H�͐F�����Ă���Ɣ����Ȋ���������̂ŁA
 * �G���{�X���H��ɃO���[�X�P�[��������Ƃ��������ɂȂ�B
 * @param uv ���݂̃s�N�Z����uv
 * @return �G���{�X���H�����F
*/
float4 Embossing(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	// �S�̂̍��v�l��1�ɂȂ�悤�ɂ���B
	// 1�ɂȂ邽�߁A���ς���K�v�Ȃ��B
	// ��Ƃ��āA����𕂂��オ�点�A�E�����ւ��܂��鏈�����s���B

	col += 2.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// ����
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));				// ��
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));		// �E��
	col += g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));				// ��
	col += g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));				// �����i�����j
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));		// �E
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));	// ����
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));		// ��
	col += -2.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));// �E��

	return col;
}

/**
 * @brief �V���[�v�l�X�B�G�b�W�̋����B
 * �e�N�X�`����g_texture�A�T���v���[��g_sampler����`����Ă��Ȃ��Ɠ����Ȃ��B
 * @param uv ���݂̃s�N�Z����uv
 * @return �V���[�v�l�X���������F
*/
float4 Sharpness(float2 uv)
{
	float w = 0.0f;
	float h = 0.0f;
	float levels = 0.0f;
	g_texture.GetDimensions(0, w, h, levels);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float distance = 2.0f;

	// �S�̂̍��v�l��1�ɂȂ�悤�ɂ���B
	// 1�ɂȂ邽�߁A���ς���K�v�Ȃ��B

	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, -distance * dy));	// ����
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, -distance * dy));		// ��
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, -distance * dy));// �E��
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, 0.0f));		// ��
	col += 5.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, 0.0f));			// �����i�����j
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, 0.0f));		// �E
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(-distance * dx, distance * dy));	// ����
	col += -1.0f * g_texture.Sample(g_sampler, uv + float2(0.0f, distance * dy));		// ��
	//col += 0.0f * g_texture.Sample(g_sampler, uv + float2(distance * dx, distance * dy));// �E��

	return col;
}