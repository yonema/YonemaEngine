#include "PeraPolygonHeader.hlsli"

SPSIn VSMain(SVSIn vsInput)
{
	SPSIn psInput;
	psInput.svpos = vsInput.pos;
	psInput.uv = vsInput.uv;

	return psInput;
}