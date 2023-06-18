#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

cbuffer Param : register(b0)
{
    float time;
    float3 dummy;
}

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    // color.rgb = PerlinNoise(pin.uv * 20.0f);
    
    // êÖÇÃï\åª
    float2 uv = pin.uv;
    uv.y -= time * 0.6f;
    float noise = PerlinNoise(uv * float2(10.0f, 1.0f));
    color.rgb = lerp(float3(0.0f, 0.0f, 0.7f), float3(0.0f, 0.5f, 1.0f), noise);
    
    color.a = 1.0f - (pin.uv.y + noise);
    
    return color;
}