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
	//--- ���Ɋւ����`
    const float Sun = 20.0f; //���z�̋����H
    const float Kr = 0.0025f; //���C���[�U���W���i�H�z���x
    const float Km = 0.001f; //�~�[�U���W���i�H�z���x
    const float KrSun = Kr * Sun;
    const float KmSun = Km * Sun;
    const float Kr4PI = Kr * 4.0f * 3.141592f;
    const float Km4PI = Km * 4.0f * 3.141592f;
	//���̎��g��
    const float3 WaveLength = float3(0.68f, 0.55f, 0.44f);
	//�U���������̋��x�͔g����4���ɔ����
    const float3 InvWaveLength = 1.0f / pow(WaveLength, 4);

	//--- ���ۂ̒n���̑傫��(km
    const float RealAtomoSphere = 100.0f;
    const float RealInnerR = 6300.0f;
    const float RealOuterR = RealAtomoSphere + RealInnerR;

	//--- �Q�[���p��scale�_�E�������n��
    const float InnerR = 100.0f;
    const float OuterR = RealOuterR * InnerR / RealInnerR;
    const float AtomoSphere = OuterR - InnerR;
    const float AtomoSphereScale = 1.0f / AtomoSphere;

	//--- �Q�[������Ԃł̃J�����̍���
    const float GameSkySize = 100.0f;
    const float CameraH = max(0.0f, camera.y);
    const float CameraHRate = CameraH / GameSkySize;

	//--- �J����
    float3 ViewRay = normalize(pin.wPos.xyz - camera);
    float3 ViewPos = float3(0.0f, InnerR + AtomoSphere * CameraHRate, 0.0f);

	//--- ��C�̈ʒu
    float a = 1.0f;
    float b = 2.0f * dot(ViewPos, ViewRay);
    float c = dot(ViewPos, ViewPos) - OuterR * OuterR;
	//TODO ----------------------
    float x = (-b + sqrt(b * b - (4.0f * a * c))) / (2.0f * a);
    float ViewLength = x;
    float3 AtomoSpherePos = ViewPos + ViewRay * ViewLength;

	//--- ���C�g
    float time = tick * 0.05f;
    float3 L = normalize(-float3(cos(time), sin(time), 0.0f));

	//---�T���v�����O�����l
    const int Samples = 2;
    float scaleOverScaleDepth = AtomoSphereScale / SCALE_DEPTH; //?
    float3 startPos = ViewPos;
    float sampleLength = ViewLength / Samples;
    float scaledLength = sampleLength * AtomoSphereScale;
    float3 sampleRay = ViewRay * sampleLength;
    float3 samplePos = startPos + sampleRay * 0.5f; //�x�N�g���̒��_���T���v�����O�ʒu�Ƃ���
	//--- ?
    float height = length(startPos);
    float startAngle = dot(ViewRay, startPos) / height;
    float startDepth = exp(scaleOverScaleDepth * (InnerR - length(startPos)));
    float startOffset = startDepth * ExpScale(startAngle);

	//--- �T���v�����O
    float3 frontColor = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < Samples; i++)
    {
        float tempHeight = length(samplePos);
        float tempDepth = exp(scaleOverScaleDepth * (InnerR - tempHeight));
        float lightAngle = dot(-L, samplePos) / tempHeight;
        float cameraAngle = dot(ViewRay, samplePos) / tempHeight;
		//���U�i�H���Ԃ�T���v�����O��ɓ����Ă�����̊O�ρj
        float scatter = startOffset + tempDepth * (ExpScale(lightAngle) - ExpScale(cameraAngle));
		//����
        float3 attenuate = exp(-scatter * (InvWaveLength * Kr4PI + Km4PI));
        frontColor += attenuate * (tempDepth * scaledLength);
        samplePos += sampleRay;
    }

	//�T���v�����O�����ʒu�ւ̓��˂������̐F
    float3 rColor = frontColor * (InvWaveLength * KrSun);
    float3 mColor = frontColor * KmSun;

	//�ʑ��֐�
    float d = dot(L, ViewRay);
    float g = -0.999f;

    float4 color = float4(0, 0, 0, 1);
    color.rgb = rColor * RayleightPhase(d) + mColor * MiePhase(d, g);
    color.a = 0.8;
	
    return color;
}