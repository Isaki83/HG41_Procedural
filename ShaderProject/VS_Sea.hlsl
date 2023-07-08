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
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.pos = float4(vin.pos, 1.0f);

    float noise = 0.0f;
    noise = Domainwork(vin.uv, 3, time);

    vout.pos.y = noise * 0.2f;
    vout.pos.y += 5.0f;
    
    vout.pos = mul(vout.pos, world);
    vout.wPos = vout.pos.xyz;

    vout.pos = mul(vout.pos, view);
    vout.pos = mul(vout.pos, proj);

	// ñ@êÚÇÃåvéZ
    float3 margin = float3(1.0f / 400, 0.0f, 1.0f / 400);

    float starX = Domainwork((vin.uv - float2(margin.x, 0.0f)), 3, time);

    float endX = Domainwork((vin.uv + float2(margin.x, 0.0f)), 3, time);

    float starZ = Domainwork((vin.uv - float2(0.0f, margin.z)), 3, time);

    float endZ = Domainwork((vin.uv + float2(0.0f, margin.z)), 3, time);

    float3 axisX = float3(margin.x, (endX - starX) * 0.5f, 0.0f);
    float3 axisZ = float3(0.0f, (endZ - starZ) * 0.5f, margin.z);
    vout.normal = normalize(cross(axisZ, axisX));

    vout.normal = mul(vout.normal, (float3x3) world);
    vout.uv = vin.uv;

    return vout;
}