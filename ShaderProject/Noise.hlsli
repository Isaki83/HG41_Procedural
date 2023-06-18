float Noise(float2 vec)
{
    return frac(sin(dot(vec, float2(12.9898f, 78.233f))) * 43758.5453123f);
}

float Noisa(float3 vec)
{
    return frac(sin(dot(vec, float3(127.9898f, 78.233f, 47.2311f))) * 43758.5453f);
}

float2 Noise2(float2 vec)
{
    return frac(sin(float2(
			dot(vec, float2(127.1f, 311.7f)),
			dot(vec, float2(269.5f, 183.3f))
		)) * 43758.5453f);
}

float3 Noise3(float3 vec)
{
    return frac(sin(float3(
			dot(vec, float3(127.1f, 311.7f, 245.4f)),
			dot(vec, float3(269.5f, 183.3f, 131.2f)),
            dot(vec, float3(522.3f, 243.1f, 532.4f))
		)) * 43758.5453f);
}

float ValueNoise(float2 vec)
{
    float2 i_uv = floor(vec);
    float2 f_uv = frac(vec);
    float2 offset[4] =
    {
        { 0.0f, 0.0f }, // 左上
        { 1.0f, 0.0f }, // 右上
        { 0.0f, 1.0f }, // 左下
        { 1.0f, 1.0f }, // 右下
    };
    
    float lt, rt, lb, rb;
    lt = Noise(i_uv + offset[0]);
    rt = Noise(i_uv + offset[1]);
    lb = Noise(i_uv + offset[2]);
    rb = Noise(i_uv + offset[3]);
    
    float top = lerp(lt, rt, f_uv.x);
    float bottom = lerp(lb, rb, f_uv.x);
    
    return lerp(top, bottom, f_uv.y);
}

float PerlinNoise(float2 vec)
{
    float2 i_uv = floor(vec);
    float2 f_uv = frac(vec);
    float2 offset[4] =
    {
        { 0.0f, 0.0f }, // 左上
        { 1.0f, 0.0f }, // 右上
        { 0.0f, 1.0f }, // 左下
        { 1.0f, 1.0f }, // 右下
    };
    
    float2 corner[4] =
    {
        Noise2(i_uv + offset[0]) * 2.0f - 1.0f,
        Noise2(i_uv + offset[1]) * 2.0f - 1.0f,
        Noise2(i_uv + offset[2]) * 2.0f - 1.0f,
        Noise2(i_uv + offset[3]) * 2.0f - 1.0f,
    };
    
    float lt, rt, lb, rb;
    lt = dot(corner[0], f_uv - offset[0]);
    rt = dot(corner[1], f_uv - offset[1]);
    lb = dot(corner[2], f_uv - offset[2]);
    rb = dot(corner[3], f_uv - offset[3]);
    
    f_uv = smoothstep(0.05f, 0.95f, f_uv);
    float top = lerp(lt, rt, f_uv.x);
    float bottom = lerp(lb, rb, f_uv.x);
    
    return (lerp(top, bottom, f_uv.y) + 1.0f) * 0.5f;
}

float PerlinNoise(float3 vec)
{
    float3 i_uv = floor(vec);
    float3 f_uv = frac(vec);
    float3 offset[8] =
    {
        { 0.0f, 0.0f, 0.0f }, // 手前左上
        { 1.0f, 0.0f, 0.0f }, // 手前右上
        { 0.0f, 1.0f, 0.0f }, // 手前左下
        { 1.0f, 1.0f, 0.0f }, // 手前右下
        { 0.0f, 0.0f, 1.0f }, // 奥左上
        { 1.0f, 0.0f, 1.0f }, // 奥右上
        { 0.0f, 1.0f, 1.0f }, // 奥左下
        { 1.0f, 1.0f, 1.0f }, // 奥右下
    };
    
    float3 corner[8] =
    {
        Noise3(i_uv + offset[0]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[1]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[2]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[3]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[4]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[5]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[6]) * 2.0f - 1.0f,
        Noise3(i_uv + offset[7]) * 2.0f - 1.0f,
    };
    
    float points[8] =
    {
        dot(corner[0], f_uv - offset[0]),
        dot(corner[1], f_uv - offset[1]),
        dot(corner[2], f_uv - offset[2]),
        dot(corner[3], f_uv - offset[3]),
        dot(corner[4], f_uv - offset[4]),
        dot(corner[5], f_uv - offset[5]),
        dot(corner[6], f_uv - offset[6]),
        dot(corner[7], f_uv - offset[7]),
    };
    
    f_uv = smoothstep(0.05f, 0.95f, f_uv);
    
    float edge[4] =
    {
        lerp(points[0], points[1], f_uv.x),
        lerp(points[2], points[3], f_uv.x),
        lerp(points[4], points[5], f_uv.x),
        lerp(points[6], points[6], f_uv.x),
    };
    
    float face[2] =
    {
        lerp(edge[0], edge[1], f_uv.y),
        lerp(edge[2], edge[3], f_uv.y),
    };
    
    return (lerp(face[0], face[1], f_uv.z) + 1.0f) * 0.5f;
}

float fBM(float2 vec, int octaves)
{
    const float lacunarity = 2.0f;  // 空隙性
    const float gain = 0.5f;        // 増加
    
    float amplitude = 0.5f;         // 振幅
    float frequency = 1.0f;         // 周波数
    
    float n = 0.0f;
    for (int i = 0; i < octaves; ++i)
    {
        n += PerlinNoise(vec * frequency) * amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    
    return n;
}