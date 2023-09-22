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
};

cbuffer WVP : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);
    vout.pos.y = fBMTurbulence(vin.uv * 1.0f, 6.0f) * 30.0f;
    vout.pos.y -= 26.0f;

    vout.pos = mul(vout.pos, world);
    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);

    //ñ@ê¸ÇÃåvéZ
    float3 margin = float3(1.0f / 200.0f, 0.0f, 1.0f / 200.0f);
    float startX = fBMTurbulence((vin.uv - float2(margin.x, 0.0f)) * 1.0f, 6.0f) * 30.0f;
    float endX = fBMTurbulence((vin.uv + float2(margin.x, 0.0f)) * 1.0f, 6.0f) * 30.0f;
    float startZ = fBMTurbulence((vin.uv - float2(0.0f, margin.z)) * 1.0f, 6.0f) * 30.0f;
    float endZ = fBMTurbulence((vin.uv + float2(0.0f, margin.z)) * 1.0f, 6.0f) * 30.0f;
    float3 axisX = float3(margin.x, (endX - startX) * 0.5f, 0.0f);
    float3 axisZ = float3(0.0f, (endZ - startZ) * 0.5f, margin.z);

    vout.normal = normalize(cross(axisZ, axisX));
    vout.normal = mul(vout.normal, (float3x3) world);
    vout.uv = vin.uv;

    return vout;
}