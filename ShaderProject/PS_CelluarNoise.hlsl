#include "Noise.hlsli"

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

cbuffer Param : register(b0)
{
    float time;
    float3 dummy;
}

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1, 1, 1, 1);
    int block = 10;
    float2 vec = pin.uv * block;
    float2 i_uv = floor(vec);
    float2 f_uv = frac(vec);

    float2 offset[] =
    {
        float2(-1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),

        float2(-1.0f, 0.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),

        float2(-1.0f, -1.0f),
		float2(0.0f, -1.0f),
		float2(1.0f, -1.0f)
	};
    float min_dist = 100.0f;
    for (int i = 0; i < 9; ++i)
    {
        float2 v = Noise2(i_uv + offset[i]);
        v = 0.5f + 0.5f * sin(time + v * 1000);
        v += offset[i];
        float dist = length(f_uv - v);
        min_dist = min(min_dist, dist);
    }

    color.rgb = pow(min_dist, 10);
    color.rgb += float4(0.0f, 1.0f, 1.0f, 1.0f);
    return color;
}