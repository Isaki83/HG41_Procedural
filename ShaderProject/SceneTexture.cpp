#include "SceneTexture.h"
#include "Sprite.h"
#include "CameraBase.h"
#include "LightBase.h"

void SceneTexture::Init()
{
	m_time = 0.0f;

	// シェーダーの読み込み処理
	const char* shaderPath[] = {
		"Assets/Shader/PS_WhiteNoise.cso",
		"Assets/Shader/PS_BlockNoise.cso",
		"Assets/Shader/PS_CelluarNoise.cso",
		"Assets/Shader/PS_ValueNoise.cso",
		"Assets/Shader/PS_PerlinNoise.cso",
		"Assets/Shader/PS_fBM.cso",
		"Assets/Shader/VS_LocalPosition.cso",
		"Assets/Shader/PS_SolidTexture.cso",
	};
	Shader* shader[] = {
		CreateObj<PixelShader>("PS_WhiteNoise"),
		CreateObj<PixelShader>("PS_BlockNoise"),
		CreateObj<PixelShader>("PS_CelluarNoise"),
		CreateObj<PixelShader>("PS_ValueNoise"),
		CreateObj<PixelShader>("PS_PerlinNoise"),
		CreateObj<PixelShader>("PS_fBM"),
		CreateObj<VertexShader>("VS_LocalPosition"),
		CreateObj<PixelShader>("PS_SolidTexture"),
	};
	for (int i = 0; i < _countof(shaderPath); ++i)
		shader[i]->Load(shaderPath[i]);

	// 立方体作成
	Vertex cvtx[] = {
		// 上の面
		{{-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},	// 0
		{{ 0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},	// 1
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},	// 2
		{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},	// 3
		// 下の面
		{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},	// 4
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},	// 5
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},	// 6
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},	// 7
		// 手前の面
		{{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},	// 8
		{{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},	// 9
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},	// 10
		{{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},	// 11
		// 左の面
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},	// 12
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},	// 13
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},	// 14
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},	// 15
		// 奥の面
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},	// 16
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},	// 17
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},	// 18
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},	// 19
		// 右の面
		{{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},},	// 20
		{{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},},	// 21
		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},},	// 22
		{{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},},	// 23
	};
	UINT idx[] = {
		 0, 1, 2,  1, 3, 2,
		 4, 5, 6,  5, 7, 6,
		 8, 9,10,  9,11,10,
		12,13,14, 13,15,14,
		16,17,18, 17,19,18,
		20,21,22, 21,23,22,
	};
	MeshBuffer::Description cdesc = {};
	cdesc.pVtx = cvtx;
	cdesc.vtxSize = sizeof(Vertex);
	cdesc.vtxCount = _countof(cvtx);
	cdesc.pIdx = idx;
	cdesc.idxSize = sizeof(idx[0]);
	cdesc.idxCount = _countof(idx);
	cdesc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	cdesc.isWrite = true;
	memcpy(m_vtx, cvtx, sizeof(cvtx));

	m_pCube = new MeshBuffer(cdesc);
}
void SceneTexture::Uninit()
{
	delete m_pCube;
}
void SceneTexture::Update(float tick)
{
	m_time += tick;

	float z = -(sinf(m_time * 0.2f) * 0.5f + 0.5f);
	m_vtx[3].pos.z = z;
	m_vtx[12].pos.z = z;
	m_vtx[17].pos.z = z;
	m_pCube->Write(m_vtx);
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
		GetObj<Shader>("PS_CelluarNoise"),
		GetObj<Shader>("PS_ValueNoise"),
		GetObj<Shader>("PS_PerlinNoise"),
		GetObj<Shader>("PS_fBM"),
	};
	shader[2]->WriteBuffer(0, time);
	shader[4]->WriteBuffer(0, time);

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

	// 立方体描画
	Shader* pVS = GetObj<Shader>("VS_LocalPosition");
	Shader* pPS = GetObj<Shader>("PS_SolidTexture");
	XMStoreFloat4x4(&mat[0], XMMatrixTranspose(XMMatrixTranslation(0.0f, 2.0f, 0.0f)));
	pVS->WriteBuffer(0, mat);
	pVS->Bind();
	pPS->Bind();
	m_pCube->Draw();
}