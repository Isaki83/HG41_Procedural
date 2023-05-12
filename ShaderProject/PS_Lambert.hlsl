struct PS_IN
{
	float4 pos : SV_POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

cbuffer Light : register(b0)
{
	float4 lightDiffuse;
	float4 lightAmbient;
	float4 lightDir;
};

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir.xyz);
	float d = saturate(dot(N, L));

	color.rgb *= saturate(d * lightDiffuse.rgb + lightAmbient.rgb);

	return color;
}