struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

cbuffer Light : register(b0)
{
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightDir;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.0f);

    float3 N = normalize(pin.normal);
    float3 L = normalize(lightDir.xyz);

    float shadow = dot(N, -L);

    color.rgb *= saturate(shadow * 0.5f + 0.5f);
    
    return color;

	//return float4(pin.uv, 0.0f, 1.0f);
}