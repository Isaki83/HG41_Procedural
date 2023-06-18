#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    color.rgb = fBM(pin.uv * 10.0f, 4);
    
    color.rgb *= 1.7f;
    color.rgb *= pow(color.rgb, 3.0f);
    color.a = color.r;
    
    return color;
}