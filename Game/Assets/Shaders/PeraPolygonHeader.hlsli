Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct SVSIn
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct SPSIn
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
};