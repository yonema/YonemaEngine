#include "SpriteHeader.hlsli"

SPSIn VSMain(SVSIn vsInput)
{
	SPSIn psInput;
	psInput.svpos = mul(g_mWorldViewProj, vsInput.pos);
	psInput.uv = vsInput.uv;

	return psInput;
}