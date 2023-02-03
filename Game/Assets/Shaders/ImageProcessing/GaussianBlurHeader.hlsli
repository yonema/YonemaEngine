#include "../SpriteHeader.hlsli"

//// �e��錾 ////

struct SPSInForBlur
{
    float4 pos : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float4 tex4 : TEXCOORD4;
    float4 tex5 : TEXCOORD5;
    float4 tex6 : TEXCOORD6;
    float4 tex7 : TEXCOORD7;
};



//// �e�탌�W�X�^ ////

cbuffer BlurCB : register(b1)
{
    float4 g_weight[2];
}



//// �֐� ////

