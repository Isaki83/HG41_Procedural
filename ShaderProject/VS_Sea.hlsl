#include "Noise.hlsli"

struct VS_IN
{
    float3 pos : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};
struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 wPos : POSITION0;
};

cbuffer WVP : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

cbuffer Param : register(b1)
{
    float3 camPos;
    float time;
}

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    float noise = 0.0f;

    //ドメインワーピング
    noise = fBM(vin.uv * 30.0f + time * 0.5f, 3);
    noise = fBM(vin.uv * 30.0f + noise + time * 0.2f, 3) * 7.0f;
    noise = fBM(vin.uv * 30.0f + time * 0.5f + noise, 3);

    vout.pos.y += noise * 0.3f;

    vout.pos = mul(vout.pos, world);
    vout.wPos = vout.pos.xyz;
    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);

    float3 margin = float3(1.0f / 200.0f, 0.0f, 1.0f / 200.0f);
    float startX = fBM((vin.uv - float2(margin.x, 0.0f)) * 30.0f + time * 0.5f, 3);
    startX = fBM((vin.uv - float2(margin.x, 0.0f)) * 30.0f + startX + time * 0.2f, 3) * 7.0f;
    startX = fBM((vin.uv - float2(margin.x, 0.0f)) * 30.0f + time * 0.5f + startX, 3);

    float endX = fBM((vin.uv + float2(margin.x, 0.0f)) * 30.0f + time * 0.5f, 3);
    endX = fBM((vin.uv + float2(margin.x, 0.0f)) * 30.0f + endX + time * 0.2f, 3) * 7.0f;
    endX = fBM((vin.uv + float2(margin.x, 0.0f)) * 30.0f + time * 0.5f + endX, 3);

    float startZ = fBM((vin.uv - float2(0.0f, margin.z)) * 30.0f + time * 0.5f, 3);
    startZ = fBM((vin.uv - float2(0.0f, margin.z)) * 30.0f + startZ + time * 0.2f, 3) * 7.0f;
    startZ = fBM((vin.uv - float2(0.0f, margin.z)) * 30.0f + time * 0.5f + startZ, 3);

    float endZ = fBM((vin.uv + float2(0.0f, margin.z)) * 30.0f + time * 0.5f, 3);
    endZ = fBM((vin.uv + float2(0.0f, margin.z)) * 30.0f + endZ + time * 0.2f, 3) * 0.7f;
    endZ = fBM((vin.uv + float2(0.0f, margin.z)) * 30.0f + time * 0.5f + endZ, 3);

    float3 axisX = float3(margin.x, (endX - startX) * 0.5f, 0.0f);
    float3 axisZ = float3(0.0f, (endZ - startZ) * 0.5f, margin.z);

    vout.normal = normalize(cross(axisZ, axisX));
    vout.normal = mul(vout.normal, (float3x3) world);
    vout.uv = vin.uv;
    return vout;
}