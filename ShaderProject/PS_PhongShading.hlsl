struct PS_IN
{
	float4 pos : SV_POSITION0;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL0; // ���f���̖@��
};

// ���C�g�̏��
cbuffer Light : register(b0)
{
	float4 lightColor; // ���̐F
	float4 lightDir; // ���̌���
};

float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// ���̌�����\���x�N�g���ƁA���f���̖@���x�N�g���̓���\��
	// �p�x�Ɋւ��āA���������Ȃ�e�ɂȂ�B���Ε������Ɩ��邭�Ȃ�
	// ���Ƃ�������B

	// ���̂܂܌v�Z����ƁA����������ʂɉe���o���Ă��܂��̂ŁA
	// �����Č��̌����𔽓]������B

	// ��̃x�N�g���̓��ς��瓯���������A���Ό��������ׂ�B
	// �����������Ȃ�1(���̒l)�A���Ό����Ȃ�-1(���̒l)
	// �F�͔���1�A����0�ɂȂ�̂ŁA���ς̌��ʂ����̂܂ܖ��邳�Ƃ��Ďg��


	return color;
}