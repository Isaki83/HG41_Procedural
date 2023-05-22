float Noise(float2 vec)
{
    return frac(sin(dot(vec, float2(12.9898f, 78.233f))) * 43758.5453123f);

}

float2 Noise2(float2 vec)
{
    return frac(sin(float2(
			dot(vec, float2(127.1f, 311.7f)),
			dot(vec, float2(269.5f, 183.3f))
		)) * 43758.5453f);
}