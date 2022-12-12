#include "PhysicsDebugLineHeader.hlsli"

float4 PSMain(SPSInput input) : SV_Target
{
	return float4(input.color, 1.0f);
}