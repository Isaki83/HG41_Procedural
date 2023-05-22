#include "SceneTexture.h"
#include "Sprite.h"
#include "CameraBase.h"
#include "LightBase.h"

using namespace DirectX;

void SceneTexture::Init()
{
	m_time = 0.0f;

	// シェーダーの読み込み処理
	const char* shaderPath[] = {
		"Assets/Shader/PS_WhiteNoise.cso",
		"Assets/Shader/PS_BlockNoise.cso",
		"Assets/Shader/PS_CelluarNoise.cso",
	};
	Shader* shader[] = {
		CreateObj<PixelShader>("PS_WhiteNoise"),
		CreateObj<PixelShader>("PS_BlockNoise"),
		CreateObj<PixelShader>("PS_CelluarNoise"),
	};
	for (int i = 0; i < _countof(shaderPath); ++i)
		shader[i]->Load(shaderPath[i]);

}
void SceneTexture::Uninit()
{
}
void SceneTexture::Update(float tick)
{
	m_time += tick;
}
void SceneTexture::Draw()
{
	// パラメータ取得
	CameraBase* pCamera = GetObj<CameraBase>("Camera");
	LightBase* pLight = GetObj<LightBase>("Light");

	// 姿勢行列の設定
	XMFLOAT4X4 mat[3];
	XMStoreFloat4x4(&mat[0], XMMatrixIdentity());
	mat[1] = pCamera->GetView();
	mat[2] = pCamera->GetProj();
	// ライトの情報
	XMFLOAT3 dir = pLight->GetDirection();
	XMFLOAT4 light[3] = {
		pLight->GetDiffuse(),
		pLight->GetAmbient(),
		{dir.x, dir.y, dir.z, 0.0f}
	};
	// 時間情報
	XMFLOAT4 time[] = {
		{m_time, 0.0f, 0.0f, 0.0f}
	};

	// シェーダー取得
	Shader* shader[] = {
		GetObj<Shader>("PS_WhiteNoise"),
		GetObj<Shader>("PS_BlockNoise"),
		GetObj<Shader>("PS_CelluarNoise")
	};
	shader[2]->WriteBuffer(0, time);

	// 描画
	Sprite::SetView(mat[1]);
	Sprite::SetProjection(mat[2]);
	int shaderNum = _countof(shader);
	for (int i = 0; i < shaderNum; ++i)
	{
		XMStoreFloat4x4(&mat[0], XMMatrixTranspose(
			XMMatrixScaling(0.95f, 0.95f, 1.0f) *
			XMMatrixTranslation(i - ((shaderNum - 1) / 2.0f), 0.5f, -0.01f)
		));
		Sprite::SetWorld(mat[0]);
		Sprite::SetPixelShader(shader[i]);
		Sprite::Draw();
	}
}