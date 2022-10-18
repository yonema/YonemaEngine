#include "BasicHeader.hlsli"

float4 BasicPS(SPSInput input) : SV_TARGET
{
	// ���C�g���
	float3 lightDir = normalize(float3(1.0f,-1.0f,1.0f));
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);


	float diffuse = dot(-lightDir, input.normal);

	float3 refLightDir = normalize(reflect(lightDir, input.normal.xyz));
	float specular = pow(saturate(dot(refLightDir, -input.cameraToPosDirWS)), g_specular.a);

	// �{���͎����x�N�g���̔��˃x�N�g���ŋ��߂邪�A�Ƃ肠����������ۂ�����B
	//float2 sphereMapUV = input.normalVS.xy;
	float2 sphereMapUV = reflect(input.cameraToPosDirVS.xy, input.normalVS.xy).xy;
	sphereMapUV = (sphereMapUV + float2(1.0f, -1.0f)) * float2(0.5f, -0.5f);
	float3 sphCol = g_sphTexture.Sample(g_sampler, sphereMapUV).xyz;
	float3 spaCol = g_spaTexture.Sample(g_sampler, sphereMapUV).xyz;

	float4 texCol = g_texture.Sample(g_sampler, input.uv);

	// �A�e�t���O�̃J���[�B
	float4 baseCol = g_diffuse * texCol;
	baseCol.xyz *= sphCol;

	float4 col = baseCol;
	col.xyz *= diffuse;
	col.xyz += spaCol;
	col.xyz += specular * g_specular.rgb;
	col.xyz = max(col.xyz, baseCol.xyz * g_ambient);


	return col;
	//return baseCol;
	return float4(sphCol, 1.0f);
}