#include "BasicModelHeader.hlsli"

cbuffer ModelCB : register(b0)
{
	float4x4 g_mWorld;
	float4x4 g_mWorldViewProj;
}

SPSInput VSMain(SVSInput input)
{
	return VSCore(input, g_mWorld, g_mWorldViewProj);
}