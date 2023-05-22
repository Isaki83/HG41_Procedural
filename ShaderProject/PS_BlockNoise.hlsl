#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1, 1, 1, 1);
    int block = 10;
    color.rgb = Noise(floor(pin.uv * block));
    return color;
}