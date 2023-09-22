struct PS_IN
{
    float4 pos : SV_POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 wPos : POSITION0;
};
cbuffer Param : register(b0)
{
    float3 camera;
    float tick;
}

float RayleightPhase(float Theta)
{
    return 3.0f / 4.0f * (1.0f + Theta * Theta);
}

float MiePhase(float Theta, float g)
{
    float g2 = g * g;
    float factor1 = 3.0f * (1.0f - g2) / 2.0f * (2.0f + g2);

    float a = 1.0f + g2 - (2.0f * g * Theta);
    float factor2 = (1.0f + Theta * Theta) / pow(a, 3.0f / 2.0f);;

    return factor1 * factor2;
}

static const float SCALE_DEPTH = 0.25f;

float ExpScale(float fcos)
{
    float x = 1.0f - fcos;
    return SCALE_DEPTH * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.8f + x * 5.25f))));
}

float4 main(PS_IN pin) : SV_TARGET
{
	//--- 光に関する定義
    const float Sun = 20.0f; //太陽の強さ？
    const float Kr = 0.0025f; //レイリー散乱係数（？吸光度
    const float Km = 0.001f; //ミー散乱係数（？吸光度
    const float KrSun = Kr * Sun;
    const float KmSun = Km * Sun;
    const float Kr4PI = Kr * 4.0f * 3.141592f;
    const float Km4PI = Km * 4.0f * 3.141592f;
	//光の周波数
    const float3 WaveLength = float3(0.68f, 0.55f, 0.44f);
	//散乱した光の強度は波長の4条に反比例
    const float3 InvWaveLength = 1.0f / pow(WaveLength, 4);

	//--- 実際の地球の大きさ(km
    const float RealAtomoSphere = 100.0f;
    const float RealInnerR = 6300.0f;
    const float RealOuterR = RealAtomoSphere + RealInnerR;

	//--- ゲーム用にscaleダウンした地球
    const float InnerR = 100.0f;
    const float OuterR = RealOuterR * InnerR / RealInnerR;
    const float AtomoSphere = OuterR - InnerR;
    const float AtomoSphereScale = 1.0f / AtomoSphere;

	//--- ゲーム内空間でのカメラの高さ
    const float GameSkySize = 100.0f;
    const float CameraH = max(0.0f, camera.y);
    const float CameraHRate = CameraH / GameSkySize;

	//--- カメラ
    float3 ViewRay = normalize(pin.wPos.xyz - camera);
    float3 ViewPos = float3(0.0f, InnerR + AtomoSphere * CameraHRate, 0.0f);

	//--- 大気の位置
    float a = 1.0f;
    float b = 2.0f * dot(ViewPos, ViewRay);
    float c = dot(ViewPos, ViewPos) - OuterR * OuterR;
	//TODO ----------------------
    float x = (-b + sqrt(b * b - (4.0f * a * c))) / (2.0f * a);
    float ViewLength = x;
    float3 AtomoSpherePos = ViewPos + ViewRay * ViewLength;

	//--- ライト
    float time = tick * 0.05f;
    float3 L = normalize(-float3(cos(time), sin(time), 0.0f));

	//---サンプリング初期値
    const int Samples = 2;
    float scaleOverScaleDepth = AtomoSphereScale / SCALE_DEPTH; //?
    float3 startPos = ViewPos;
    float sampleLength = ViewLength / Samples;
    float scaledLength = sampleLength * AtomoSphereScale;
    float3 sampleRay = ViewRay * sampleLength;
    float3 samplePos = startPos + sampleRay * 0.5f; //ベクトルの中点をサンプリング位置とする
	//--- ?
    float height = length(startPos);
    float startAngle = dot(ViewRay, startPos) / height;
    float startDepth = exp(scaleOverScaleDepth * (InnerR - length(startPos)));
    float startOffset = startDepth * ExpScale(startAngle);

	//--- サンプリング
    float3 frontColor = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < Samples; i++)
    {
        float tempHeight = length(samplePos);
        float tempDepth = exp(scaleOverScaleDepth * (InnerR - tempHeight));
        float lightAngle = dot(-L, samplePos) / tempHeight;
        float cameraAngle = dot(ViewRay, samplePos) / tempHeight;
		//分散（？たぶんサンプリング一に入ってくる光の外積）
        float scatter = startOffset + tempDepth * (ExpScale(lightAngle) - ExpScale(cameraAngle));
		//減衰
        float3 attenuate = exp(-scatter * (InvWaveLength * Kr4PI + Km4PI));
        frontColor += attenuate * (tempDepth * scaledLength);
        samplePos += sampleRay;
    }

	//サンプリングした位置への入射した光の色
    float3 rColor = frontColor * (InvWaveLength * KrSun);
    float3 mColor = frontColor * KmSun;

	//位相関数
    float d = dot(L, ViewRay);
    float g = -0.999f;

    float4 color = float4(0, 0, 0, 1);
    color.rgb = rColor * RayleightPhase(d) + mColor * MiePhase(d, g);
    color.a = 0.8;
	
    return color;
}