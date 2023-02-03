#include "BasicModelHeader.hlsli"
#include "../CalcSkinMatrixHeader.hlsli"


/**
 * @brief 頂点シェーダーのコア処理
*/
SPSInput VSMainCore(in SVSInput input, in float4x4 mWorldLocal)
{
	SPSInput output =
	{
		{ 0.0f, 0.0f, 0.0f, 0.0f },	// svpos
		{ 0.0f, 0.0f, 0.0f },		// normal
		{ 0.0f, 0.0f, 0.0f },		// tangent
		{ 0.0f, 0.0f, 0.0f },		// biNormal
		{ 0.0f, 0.0f },				// uv
		{ 0.0f, 0.0f,0.0f, 0.0f },	// posInLVP
		{ 0.0f, 0.0f,0.0f, 0.0f },	// posInLVP
	};

	output.posInWorld = mul(mWorldLocal, input.pos);
	output.svpos = mul(g_mViewProj, output.posInWorld);

	output.posInLVP = mul(g_mLightViewProj, output.posInWorld);


	// 法線関係は、平行移動成分を除去する。
	float3x3 mWorldLocal3x3 = (float3x3)mWorldLocal;
	output.normal = normalize(mul(mWorldLocal3x3, input.normal));
	output.tangent = normalize(mul(mWorldLocal3x3, input.tangent));
	output.biNormal = normalize(mul(mWorldLocal3x3, input.biNormal));

	output.uv = input.uv;



	return output;
}


//// 頂点シェーダーの各種エントリーポイント ////

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