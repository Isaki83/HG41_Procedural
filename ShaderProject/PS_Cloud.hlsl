#include	"Noise.hlsli"


struct PS_IN
{
    float4 pos : SV_POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 wPos : POSITION0;
};

// カメラバッファ
cbuffer Param : register(b0)
{
    float3 camPos;
    float time;
}

// ライトバッファ
cbuffer Light : register(b1)
{
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightDir;
};

float4 main(PS_IN pin) : SV_TARGET
{
    /* ---設定用パラメータ--- */
    int MaxSample = 20; // 繰り返し回数
    int MaxShadowSample = 3; // 雲の陰のサンプリング回数

    float sampleLen = 3.73f; // 立方体の辺(対角線)の長さ(√3)
    float shadowSampleLen = sampleLen * 0.5f;

    float step = sampleLen / MaxSample; // サンプリング の進む長さ(一辺の長さをサンプリング数で割る)
    float shadowStep = shadowSampleLen / MaxShadowSample; // 陰用のサンプリングの進む長さ

    float3 boxCenter = float3(0.0f, 15.0f, 0.0f);
    float3 boxSize = float3(30.0f, 10.0f, 30.0f);

    /* ---計算用パラメータ--- */
    float3 pos = pin.wPos.xyz;
    float3 stepDir = normalize(pos - camPos);
    float3 stepLDir = normalize(-lightDir.xyz);

    // 模様を低減をするための計算
    // ボックスの表面から計算を行うのではなく
    // カメラからStep間隔で配置された平面事に行う。

    // カメラからのピクセルまでの距離
    float camDist = length(camPos - pos);

    // ステップが何回実行できるか
    float stepNum = camDist / step;

    // 補正した場合の開始までの距離
    float startDist = (floor(stepNum) + 1.0f) * step;

    // 補正した場合の開始位置
    pos = camPos + stepDir * startDist;

    /* ---雲の濃度(不透明度)を計算--- */
    float trans = 0.0f;
    float totalLight = 0.0f;

    for (int i = 0; i < MaxSample; ++i)
    {

        // ボックス中心からの距離
        float3 diff = abs(pos - boxCenter);
        float3 hSize = boxSize * 0.5f;

        // 端に近い程薄くなるように計算
        float3 edgeDist = saturate(hSize - diff);
        edgeDist /= 0.4f;
        edgeDist = pow(edgeDist, 2);
        edgeDist = saturate(edgeDist);
        float rate = edgeDist.x * edgeDist.y * edgeDist.z;

        // 雲の密度を取得
        float denstiy = saturate(fBM(pos * /* 密度の大きさを変えれる */0.8f + time * 1.2f, 3) * 2.3f - 0.7f) * /* ここの値が雲の濃さを変えれる */0.5f;
        trans += denstiy;


        /* ---雲の陰の計算(光がどれだけ届くか)--- */
        float3 lPos = pos;
        float lightPower = 1.0f;

        for (int j = 0; j < MaxShadowSample; ++j)
        {

            // 光の反射率
            float reflection = saturate(fBM(lPos * 0.8f + time, 3) * 2.3f - 0.8f) * 0.2f;
            lightPower *= 1.0f - reflection;

            // サンプリング位置の移動
            lPos += stepDir * shadowStep;
        }

        totalLight += lightPower;

        // 次のサンプリング位置へ移動
        pos += stepDir * step;

        /* ---これ以上見通せなければ終了--- */
        if (trans >= 1.0f)
        {
            break;
        }

        ///* ---サンプル位置が範囲外かの判定--- */
        //diff = abs(pos - boxCenter);
        //hSize = boxSize * 0.5f;

        if (diff.x > hSize.x || diff.y > hSize.y || diff.z > hSize.z)
        {
            break;
        }

    }

    totalLight /= MaxSample;

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    color.rgb *= totalLight;
    color.g += saturate((1.0f - totalLight) * 0.1f);
    color.r += saturate((1.0f - totalLight) * 0.3f);
    color.a *= trans;

    return color;
}
