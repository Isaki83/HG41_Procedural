#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 wPos : TEXCOORD1;
};

cbuffer Light : register(b0)
{
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightDir;
};
cbuffer Parm : register(b1)
{
    float3 camPos;
    float time;
}
float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0.2f, 0.5f, 0.2f, 1.0f);

    float noise = Domainwork(pin.uv, 3, time);

    color.rgb = lerp(float3(0.0f, 0.1f, 0.6f), float3(0.0f, 0.1f, 0.6f), noise * 10);
	//color.rgb = float3(0.0f, 0.1f, 0.6f) * noise;
    
    float3 eye = camPos - pin.wPos;
    float3 L = normalize(lightDir.xyz);
    float3 N = normalize(pin.normal);
    float3 V = normalize(eye);

    float dotNV = saturate(dot(N, V));
    
	// îΩéÀåWêî
    float f0 = 0.01f;
    float fresnel = f0 + (1.0f - f0) * pow(1.0f - dotNV, 5.0f);
    color.a = fresnel + 0.2f;
    color.a += dot(L, N) * 0.01f;

	// ãæñ îΩéÀ
    float3 R = reflect(-V, N);
    float3 dotLR = saturate(dot(-L, R));
    dotLR = pow(dotLR, 20.0f);

    color.rgb += dotLR * 20;

    return color;

	//return float4(pin.uv, 0.0f, 1.0f);
}