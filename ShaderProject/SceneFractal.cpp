#include "SceneFractal.h"
#include "Model.h"
#include "CameraBase.h"
#include "LightBase.h"
#include "Input.h"

using namespace DirectX;

void SceneFractal::Init()
{
	// シェーダーの読み込み処理
	const char* shaderPath[] = {
		"Assets/Shader/VS_Object.cso",
		"Assets/Shader/PS_Color.cso",
		"Assets/Shader/PS_TexColor.cso",
		"Assets/Shader/PS_Lambert.cso",
	};
	Shader* shader[] = {
		CreateObj<VertexShader>("VS_Object"),
		CreateObj<PixelShader>("PS_Color"),
		CreateObj<PixelShader>("PS_TexColor"),
		CreateObj<PixelShader>("PS_Lambert"),
	};
	for (int i = 0; i < _countof(shaderPath); ++i)
		shader[i]->Load(shaderPath[i]);

	// 頂点バッファの作成
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
	};
	Vertex vtx[] = {
		{{ 0.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	};
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx;
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = _countof(vtx);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pTriangle = new MeshBuffer(desc);

	// 立方体作成
	Vertex cvtx[] = {
		// 上の面
		{{-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},
		{{ 0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},
		{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},
		// 下の面
		{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},
		// 手前の面
		{{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		// 左の面
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},
		// 奥の面
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		// 右の面
		{{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},},
		{{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},},
		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},},
		{{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},},
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
	m_pCube = new MeshBuffer(cdesc);
}
void SceneFractal::Uninit()
{
	delete m_pCube;
	delete m_pTriangle;
}
void SceneFractal::Update(float tick)
{
}
void SceneFractal::Draw()
{
	// GetObj 引数で指定された名前のデータを
	//        テンプレート引数に指定された型に変換して取得
	CameraBase* pCamera = GetObj<CameraBase>("Camera");
	LightBase* pLight = GetObj<LightBase>("Light");
	// シェーダー取得
	Shader* shader[] = {
		GetObj<VertexShader>("VS_Object"),
		GetObj<PixelShader>("PS_Color"),
		GetObj<PixelShader>("PS_TexColor"),
		GetObj<PixelShader>("PS_Lambert"),
	};

	// 姿勢行列の情報
	XMFLOAT4X4 mat[3];
	XMStoreFloat4x4(&mat[0], XMMatrixIdentity());
	mat[1] = pCamera->GetView();
	mat[2] = pCamera->GetProj();
	// カメラの情報
	XMFLOAT3 dir = pLight->GetDirection();
	XMFLOAT4 light[3] = {
		pLight->GetDiffuse(),
		pLight->GetAmbient(),
		{dir.x, dir.y, dir.z, 0.0f}
	};

	// データ書き込み
	shader[0]->WriteBuffer(0, mat);
	shader[3]->WriteBuffer(0, light);

	// 描画
	shader[0]->Bind();
	shader[1]->Bind();
	int depth = 3;
	//DrawTriangle(depth, depth, 0.0f, 0.0f);

	shader[3]->Bind();
	DrawCube(depth, depth, 0.0f, 0.0f, 0.0f);
}

void SceneFractal::DrawTriangle(int maxDepth, int depth, float x, float y)
{
	if (depth > 0) 
	{
		// 三角形に穴を開けて移動
		/*
		   --- 三角形に穴を開けるたびに、移動量が1/2になる
		   --- 1回→1/4  2回→1/8  3回→1/16 ...
		*/
		float dist = 1.0f / powf(2.0f, 2.0f + (maxDepth - depth));
		// 上に移動
		DrawTriangle(maxDepth, depth - 1, x + 0.0f, y + dist);
		// 左下に移動
		DrawTriangle(maxDepth, depth - 1, x - dist, y - dist);
		// 右下に移動
		DrawTriangle(maxDepth, depth - 1, x + dist, y - dist);
	}
	else 
	{
		// 描画
		Shader* pvs = GetObj<VertexShader>("VS_Object");
		CameraBase* pCamera = GetObj<CameraBase>("Camera");
		XMFLOAT4X4 mat[3];
		float scale = 1.0f / powf(2.0f, maxDepth);
		XMStoreFloat4x4(&mat[0], XMMatrixTranspose(
			XMMatrixScaling(scale, scale, scale) *
			XMMatrixTranslation(x, y, 0.0f)
		));
		mat[1] = pCamera->GetView();
		mat[2] = pCamera->GetProj();
		pvs->WriteBuffer(0, mat);
		m_pTriangle->Draw();
	}
}

void SceneFractal::DrawCube(int maxDepth, int depth, float x, float y, float z)
{
	if (depth > 0)
	{
		// 穴あけする処理
		// 立方体を分割したそれぞれの箇所で表示するかしないかを定義
		int idx[] = {
			1,1,1, 1,0,1, 1,1,1,	// 上段
			1,0,1, 0,0,0, 1,0,1,	// 中断
			1,1,1, 1,0,1, 1,1,1,	// 下段
		};
		float dist = 1.0f / (powf(3.0f, (maxDepth - depth) + 1.0f));
		for (int i = 0; i < _countof(idx); ++i)
		{
			if (!idx[i]) { continue; }
			DrawCube(maxDepth, depth - 1,
				x + (i % 3 - 1) * dist,
				y + ((i / 3) % 3 -1) * dist,
				z + (i / 9 - 1) * dist);
		}
	}
	else
	{
		// 描画
		CameraBase* pCamera = GetObj<CameraBase>("Camera");
		XMFLOAT4X4 mat[3];
		float scale = 1.0f / powf(3.0f, maxDepth);
		XMStoreFloat4x4(&mat[0], XMMatrixTranspose(
			XMMatrixScaling(scale, scale, scale) *
			XMMatrixTranslation(x, y, z)
		));
		mat[1] = pCamera->GetView();
		mat[2] = pCamera->GetProj();
		Shader* pvs = GetObj<VertexShader>("VS_Object");
		pvs->WriteBuffer(0, mat);
		m_pCube->Draw();
	}
}