#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 localPos : TEXCOORD1;
};

cbuffer Param : register(b0)
{
    float time;
    float3 dummy;
}


float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0.0f, 5.0f, 0.0f, 1.0f);

    float2 uv = pin.uv;
    uv.y += time * 0.1f;
    float n = PerlinNoise(pin.localPos * 10.0f);

	//color.rgb = n;
	//color.rg = 1.0f;
	//color.rgb = n;
	//color.r = 0.0f;
	//color.g = 0.0f;
	//color.b = pow(n * 2.5, 50.0f);
	
    color.r = 0;
    color.g = 0;
    color.b = pow(n * 2, 50.0f) * 10;
    return color;

	//float4 color = float4(1.0f,1.0f,1.0f,1.0f);

	//float2 uv = pin.uv;
	//uv.y += time * 0.6f;
	
	//float n = PerlinNoise(pin.localPos * 1.0f);
	//float noise = PerlinNoise(pin.localPos * float2(0.01, 0.1) * uv);
	
	//color.rgb = n;
	//color.rgb = pow(n * 2, 20.0f);
	//color.rgb = lerp(float3(0.0f, 0.0f, 0.7f), float3(0.0f, 0.5f, 0.7f), noise);

	//return color;
}