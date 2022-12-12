
/*!
 * @brief 頂点シェーダーへ入ってくるデータ
 */
struct SVSInput
{
	float3 pos : POSITION;
	float3 color : COLOR;
};

/*!
 * @brief 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
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
