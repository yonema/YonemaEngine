
// #include "ShadowConstDataHeader.hlsli"

/**
 * @brief �`�F�r�V�F�t�̕s�����̃R�A�֐�
*/
float ChebyshevCore(in float2 moments, in float depth)
{
    // �Օ�����Ă���Ȃ�A�`�F�r�V�F�t�̕s�����𗘗p���Č���������m�������߂�
    float depth_sq = moments.x * moments.x;
    // ���̃O���[�v�̕��U������߂�
    // ���U���傫���قǁAvariance�̐��l�͑傫���Ȃ�
    float variance = moments.y - depth_sq;
    // ���̃s�N�Z���̃��C�g���猩���[�x�l�ƃV���h�E�}�b�v�̕��ς̐[�x�l�̍������߂�
    float md = depth - moments.x;
    // �����͂��m�������߂�
    float lit_factor = variance / (variance + md * md);
    float lig_factor_min = 0.3f;
    // �����͂��m���̉����ȉ��͉e�ɂȂ�悤�ɂ���B
    lit_factor = saturate((lit_factor - lig_factor_min) / (1.0f - lig_factor_min));
    // �����͂��m������e�ɂȂ�m�������߂�B
    return 1.0f - lit_factor;
}

/**
 * @brief �`�F�r�V�F�t�̕s�����𗘗p���āA�e�ɂȂ�\�����v�Z����B
*/
float Chebyshev(in float2 moments, in float depth)
{
    // �������^�[������ƌx�����o�邽�߁A�֐���2�ɕ����đΉ��B

    float shadowRate = 0.0f;

    if (depth > moments.x)
    {
        shadowRate = ChebyshevCore(moments, depth);
    }


    return shadowRate;
}


float2 CalcShadowMapUVFromPosInLVP(in float4 posInLVP)
{
    // ���C�g�r���[�v���W�F�N�V�����ł̍��W�𐳋K������(-1.0f�`1.0f)�ɂ���
    float2 shadowMapUV = posInLVP.xy / posInLVP.w;
    // ���K���X�N���[�����W�n(-1.0f�`1.0f)����(-0.5f�`0.5f)�ɕϊ�
    // Y���W�͔��]������
    shadowMapUV *= float2(0.5f, -0.5f);
    // (-0.5f�`0.5f)����UV���W�n(0.0f�`1.0f)�ɕϊ�
    shadowMapUV += 0.5f;

    return shadowMapUV;
}

/**
 * @brief �e�ɂȂ�\���̌v�Z�̃R�A�֐�
*/
float CalcShadowRateCore(
    in Texture2D<float4> shadowMap,
    in float2 shadowMapUV,
    float zInLVP,
    in bool isSoftShadow
)
{
    float shadowRate = 0.0f;

    float4 shadowValue = shadowMap.Sample(g_sampler, shadowMapUV);

    if (shadowValue.x > 0.00000001f)
    {
#ifdef EVSM
        //zInLVP = exp(g_kExp * zInLVP);
        zInLVP = exp(g_kExp * (zInLVP - g_kDepthBias));
#else
        zInLVP -= g_kDepthBias;
#endif  // EVSM

        if (isSoftShadow)
        {
            // �\�t�g�V���h�E�B
            shadowRate = Chebyshev(shadowValue.xy, zInLVP);
        }
        else if (zInLVP >= shadowValue.r)
        {
            // �n�[�h�V���h�E�B
            shadowRate = 1.0f;
        }
    }

    return shadowRate;
}

/**
 * @brief �e�ɂȂ�\�����v�Z����B
*/
float CalcShadowRate(
    in Texture2D<float4> shadowMap,
    in float4 posInLVP,
    in float zInLVP,
    in bool isSoftShadow
)
{
    // �������^�[������ƌx�����o�邽�߁A�֐���2�ɕ����đΉ��B

    float shadowRate = 0.0f;

    float2 shadowMapUV = CalcShadowMapUVFromPosInLVP(posInLVP);

    // �V���h�E�}�b�vUV�ƃ��C�g�r���[�v���W�F�N�V�����̐[�x����A
    // �V���h�E�}�b�v�͈͓̔������ׂ�B
    if (shadowMapUV.x >= 0.0f && shadowMapUV.x <= 1.0f &&
        shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f &&
        zInLVP >= 0.0f && zInLVP <= 1.0f)
    {
        // �V���h�E�}�b�v�͈͓̔�
        shadowRate = CalcShadowRateCore(shadowMap, shadowMapUV, zInLVP, isSoftShadow);

        // �V���h�E�}�b�v�̒[�̕��́A���X�ɉe�𔖂�����B
        if (shadowRate > 0.0000001f)
        {
            float fromCenterDist = length(shadowMapUV - float2(0.5f, 0.5f));
            // ���K��
            fromCenterDist /= g_k1DivRoot2;
            float distBias = saturate(1.0f - pow(fromCenterDist, g_kDistBiasPower));
            shadowRate *= distBias;
        }
    }

    return shadowRate;
}