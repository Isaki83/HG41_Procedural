#include	"Noise.hlsli"


struct PS_IN
{
    float4 pos : SV_POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float4 wPos : POSITION0;
};

// �J�����o�b�t�@
cbuffer Param : register(b0)
{
    float3 camPos;
    float time;
}

// ���C�g�o�b�t�@
cbuffer Light : register(b1)
{
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightDir;
};

float4 main(PS_IN pin) : SV_TARGET
{
    /* ---�ݒ�p�p�����[�^--- */
    int MaxSample = 20; // �J��Ԃ���
    int MaxShadowSample = 3; // �_�̉A�̃T���v�����O��

    float sampleLen = 3.73f; // �����̂̕�(�Ίp��)�̒���(��3)
    float shadowSampleLen = sampleLen * 0.5f;

    float step = sampleLen / MaxSample; // �T���v�����O �̐i�ޒ���(��ӂ̒������T���v�����O���Ŋ���)
    float shadowStep = shadowSampleLen / MaxShadowSample; // �A�p�̃T���v�����O�̐i�ޒ���

    float3 boxCenter = float3(0.0f, 15.0f, 0.0f);
    float3 boxSize = float3(30.0f, 10.0f, 30.0f);

    /* ---�v�Z�p�p�����[�^--- */
    float3 pos = pin.wPos.xyz;
    float3 stepDir = normalize(pos - camPos);
    float3 stepLDir = normalize(-lightDir.xyz);

    // �͗l��ጸ�����邽�߂̌v�Z
    // �{�b�N�X�̕\�ʂ���v�Z���s���̂ł͂Ȃ�
    // �J��������Step�Ԋu�Ŕz�u���ꂽ���ʎ��ɍs���B

    // �J��������̃s�N�Z���܂ł̋���
    float camDist = length(camPos - pos);

    // �X�e�b�v��������s�ł��邩
    float stepNum = camDist / step;

    // �␳�����ꍇ�̊J�n�܂ł̋���
    float startDist = (floor(stepNum) + 1.0f) * step;

    // �␳�����ꍇ�̊J�n�ʒu
    pos = camPos + stepDir * startDist;

    /* ---�_�̔Z�x(�s�����x)���v�Z--- */
    float trans = 0.0f;
    float totalLight = 0.0f;

    for (int i = 0; i < MaxSample; ++i)
    {

        // �{�b�N�X���S����̋���
        float3 diff = abs(pos - boxCenter);
        float3 hSize = boxSize * 0.5f;

        // �[�ɋ߂��������Ȃ�悤�Ɍv�Z
        float3 edgeDist = saturate(hSize - diff);
        edgeDist /= 0.4f;
        edgeDist = pow(edgeDist, 2);
        edgeDist = saturate(edgeDist);
        float rate = edgeDist.x * edgeDist.y * edgeDist.z;

        // �_�̖��x���擾
        float denstiy = saturate(fBM(pos * /* ���x�̑傫����ς���� */0.8f + time * 1.2f, 3) * 2.3f - 0.7f) * /* �����̒l���_�̔Z����ς���� */0.5f;
        trans += denstiy;


        /* ---�_�̉A�̌v�Z(�����ǂꂾ���͂���)--- */
        float3 lPos = pos;
        float lightPower = 1.0f;

        for (int j = 0; j < MaxShadowSample; ++j)
        {

            // ���̔��˗�
            float reflection = saturate(fBM(lPos * 0.8f + time, 3) * 2.3f - 0.8f) * 0.2f;
            lightPower *= 1.0f - reflection;

            // �T���v�����O�ʒu�̈ړ�
            lPos += stepDir * shadowStep;
        }

        totalLight += lightPower;

        // ���̃T���v�����O�ʒu�ֈړ�
        pos += stepDir * step;

        /* ---����ȏ㌩�ʂ��Ȃ���ΏI��--- */
        if (trans >= 1.0f)
        {
            break;
        }

        ///* ---�T���v���ʒu���͈͊O���̔���--- */
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
