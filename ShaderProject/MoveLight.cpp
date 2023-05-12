#include "MoveLight.h"
#include <algorithm>
#include "Input.h"


MoveLight::MoveLight()
	: m_radXZ(0.0f)
	, m_radY(DirectX::XM_PIDIV4)
	, m_h(0.0f)
	, m_sv(1.0f)
{
}
MoveLight::~MoveLight()
{
}

void MoveLight::Update()
{
	if (IsKeyPress('L'))
		UpdateParam();
}
void MoveLight::UpdateParam()
{
	// âÒì]
	constexpr float rotSpeed = DirectX::XMConvertToRadians(360.0f / (2.0f * 60)); // 2ïbÇ≈1âÒì]
	if (IsKeyPress('A')) { m_radXZ += rotSpeed; }
	if (IsKeyPress('D')) { m_radXZ -= rotSpeed; }
	if (IsKeyPress('W')) { m_radY += rotSpeed; }
	if (IsKeyPress('S')) { m_radY -= rotSpeed; }
	m_radY = std::min(std::max(m_radY, 0.0f), DirectX::XM_PIDIV2);

	const float lightLength = 5.0f;
	m_direction.x = cosf(m_radY) * sinf(m_radXZ) * lightLength;
	m_direction.y = -sinf(m_radY) * lightLength;
	m_direction.z = cosf(m_radY) * cosf(m_radXZ) * lightLength;

	m_pos.x = -m_direction.x;
	m_pos.y = -m_direction.y;
	m_pos.z = -m_direction.z;

	// êFïœçX
}

void MoveLight::SetRot(float xz, float y)
{
	m_radXZ = xz;
	m_radY = y;
}
float MoveLight::GetRotXZ()
{
	return m_radXZ;
}
float MoveLight::GetRotY()
{
	return m_radY;
}

void MoveLight::SetHSV(float h, float sv)
{
	m_h = h;
	m_sv = sv;
	//m_diffuse = HSV2Color(m_h, m_sv);
}
float MoveLight::GetH()
{
	return m_h;
}
float MoveLight::GetSV()
{
	return m_sv;
}

DirectX::XMFLOAT4 HSV2Color(float h, float sv)
{
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);

	sv = sv * 2.0f - 1.0f;
	float s = 1.0f - std::max(sv, 0.0f);
	float v = std::min(sv, 0.0f) + 1.0f;


	return color;
}