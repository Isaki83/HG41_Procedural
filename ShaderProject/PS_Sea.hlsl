#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 wPos : POSITION0;
};

cbuffer Light : register(b0)
{
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightDir;
};

cbuffer Param : register(b1)
{
    float3 camPos;
    float time;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    float noise = 0.0f;
    /*ここにドメインワーピングの処理*/
    noise = fBM(pin.uv * 30.0f + time * 0.5f, 3);
    noise = fBM(pin.uv * 30.0f + noise + time * 0.2f, 3) * 7.0f;
    noise = fBM(pin.uv * 30.0f + time * 0.5f + noise, 3);

    color.rgb = lerp(
        float3(0.0f, 0.1f, 0.6f),
        float3(0.1f, 0.6f, 0.8f),
        noise);

    float3 eye = camPos - pin.wPos;
    float3 L = normalize(lightDir.xyz);
    float3 N = normalize(pin.normal);
    float3 V = normalize(eye);

    float dotNV = saturate(dot(N, V));

    float f0 = 0.8f;
    float fresnel = f0 + (1.0f - f0) * pow(1.0f - dotNV, 5.0f);
    
    //鏡面反射（太陽の反射）
    float3 R = reflect(-V, N);
    float dotLR = max(0, dot(-L, R));
    dotLR = pow(dotLR, 30.0f);

    color.rgb += dotLR;
    color.a = fresnel + dot(L, N) * 0.01f - 0.1f;

    return color;
}