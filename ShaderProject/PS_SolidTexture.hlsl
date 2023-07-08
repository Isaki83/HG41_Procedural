#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 localPos : TEXCOORD1;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float n = PerlinNoise(pin.localPos * 10.0f);
    color.rgb = pow(n * 2, 20.0f);
    
    return color;
}