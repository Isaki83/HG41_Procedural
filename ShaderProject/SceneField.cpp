#include "SceneField.h"
#include "Shader.h"
#include "CameraBase.h"
#include "LightBase.h"

void SceneField::Init()
{
	// 初期値設定
	m_time = 0.0f;

	// シェーダーの読み込み処理
	const char* shaderPath[] = {
		"Assets/Shader/VS_Field.cso",
		"Assets/Shader/PS_Field.cso",
		"Assets/Shader/VS_Sea.cso",
		"Assets/Shader/PS_Sea.cso",
		"Assets/Shader/VS_WorldPostion.cso",
		"Assets/Shader/PS_Atomosphere.cso",
		"Assets/Shader/PS_Cloud.cso"
	};
	Shader* shader[] = {
		CreateObj<VertexShader>("VS_Field"),
		CreateObj<PixelShader>("PS_Field"),
		CreateObj<VertexShader>("VS_Sea"),
		CreateObj<PixelShader>("PS_Sea"),
		CreateObj<VertexShader>("VS_WorldPostion"),
		CreateObj<PixelShader>("PS_Atomosphere"),
		CreateObj<PixelShader>("PS_Cloud")
	};
	for (int i = 0; i < _countof(shaderPath); ++i)
		shader[i]->Load(shaderPath[i]);

	//--- 平面作成
	// 平面定義
	struct Vertex
	{
		float pos[3];
		float normal[3];
		float uv[2];
	};
	const float maxSize = 20.0f;
	const int GridNum = 200;
	const float planeSpace = maxSize / (GridNum - 1);
	// 頂点生成
	std::vector<Vertex> planeVtx;
	for (int j = 0; j < GridNum; ++j)
	{
		for (int i = 0; i < GridNum; ++i)
		{
			planeVtx.push_back(
				{
					{i * planeSpace - maxSize * 0.5f, 0.0f, j * planeSpace - maxSize * 0.5f},
					{0.0f, 1.0f, 0.0f},
					{ i / (GridNum - 1.0f), j / (GridNum - 1.0f) }
				}
			);
		}
	}
	// インデックス生成
	std::vector<DWORD> planeIdx;
	for (int j = 0; j < GridNum - 1; ++j)
	{
		for (int i = 0; i < GridNum - 1; ++i)
		{
			planeIdx.push_back(GridNum * j + i);
			planeIdx.push_back(GridNum * j + i + 1);
			planeIdx.push_back(GridNum * (j + 1) + i);
			planeIdx.push_back(GridNum * (j + 1) + i);
			planeIdx.push_back(GridNum * j + i + 1);
			planeIdx.push_back(GridNum * (j + 1) + i + 1);
		}
	}
	// 頂点バッファ作成
	MeshBuffer::Description desc = {};
	desc.pVtx = planeVtx.data();
	desc.vtxCount = planeVtx.size();
	desc.vtxSize = sizeof(Vertex);
	desc.pIdx = planeIdx.data();
	desc.idxCount = planeIdx.size();
	desc.idxSize = sizeof(DWORD);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_obj.push_back(new MeshBuffer(desc));

	//球面
	const int SPHERE_HORIZONTAL = 17;
	const int SPHERE_VERTICAL = 9;
	const float SPHERE_SIZE = 10.0f;
	std::vector<Vertex> sphereVtx;

	for (int j = 0; j < SPHERE_VERTICAL; ++j)
	{
		float radY = DirectX::XMConvertToRadians(180.0f * j / (SPHERE_VERTICAL - 1));
		float sinY = sinf(radY);
		float cosY = cosf(radY);
		for (int i = 0; i < SPHERE_HORIZONTAL; i++)
		{
			float rad = DirectX::XMConvertToRadians(360.0f * i / (SPHERE_HORIZONTAL - 1));
			sphereVtx.push_back({
				{ sinY * sinf(rad) * SPHERE_SIZE,cosY * SPHERE_SIZE,sinY * cosf(rad) * SPHERE_SIZE },
				{ sinY * sinf(rad),cosY,sinY * cosf(rad) },
				{ i / (SPHERE_HORIZONTAL - 1.0f),j / (SPHERE_VERTICAL - 1.0f) } });
		}
	}
	//インデックス
	std::vector<unsigned long> sphereIdx;
	for (int j = 0; j < SPHERE_VERTICAL - 1; ++j)
	{
		for (int i = 0; i < SPHERE_HORIZONTAL - 1; i++)
		{
			sphereIdx.push_back((j + 1) * SPHERE_HORIZONTAL + i);
			sphereIdx.push_back(j * SPHERE_HORIZONTAL + i);
			sphereIdx.push_back(j * SPHERE_HORIZONTAL + i + 1);
			sphereIdx.push_back(j * SPHERE_HORIZONTAL + i + 1);
			sphereIdx.push_back((j + 1) * SPHERE_HORIZONTAL + i + 1);
			sphereIdx.push_back((j + 1) * SPHERE_HORIZONTAL + i);
		}
	}
	//バッファの作成
	desc.pVtx = sphereVtx.data();
	desc.vtxCount = sphereVtx.size();
	desc.pIdx = sphereIdx.data();
	desc.idxCount = sphereIdx.size();
	desc.idxSize = sizeof(int);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_obj.push_back(new MeshBuffer(desc));

	//立方体の作成
	Vertex cvtx[]
	{
		//上の面
		{{-0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f},{ 0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 1.0f, 0.0f}},

		//下の面
		{{-0.5f,-0.5f, 0.5f},{ 0.0f,-1.0f, 0.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 0.0f,-1.0f, 0.0f}},
		{{-0.5f,-0.5f,-0.5f},{ 0.0f,-1.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 0.0f,-1.0f, 0.0f}},

		//手前の面
		{{-0.5f, 0.5f,-0.5f},{ 0.0f, 0.0f,-1.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 0.0f, 0.0f,-1.0f}},
		{{-0.5f,-0.5f,-0.5f},{ 0.0f, 0.0f,-1.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 0.0f, 0.0f,-1.0f}},

		//奥の面
		{{-0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f}},
		{{-0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f}},
		{{ 0.5f,-0.5f, 0.5f},{ 0.0f, 0.0f, 1.0f}},

		//右側の面
		{{ 0.5f,-0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f}},
		{{ 0.5f, 0.5f, 0.5f},{ 1.0f, 0.0f, 0.0f}},
		{{ 0.5f,-0.5f,-0.5f},{ 1.0f, 0.0f, 0.0f}},
		{{ 0.5f, 0.5f,-0.5f},{ 1.0f, 0.0f, 0.0f}},

		//左側の面
		{{-0.5f,-0.5f, 0.5f},{-1.0f, 0.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f},{-1.0f, 0.0f, 0.0f}},
		{{-0.5f,-0.5f,-0.5f},{-1.0f, 0.0f, 0.0f}},
		{{-0.5f, 0.5f,-0.5f},{-1.0f, 0.0f, 0.0f}},
	};

	WORD idx[] =
	{ 0, 1, 2, 1, 3, 2,
	  4, 5, 6, 5, 7, 6,
	  8, 9,10, 9,11,10,
	 12,13,14,13,15,14,
	 16,17,18,17,19,18,
	 20,21,22,21,23,22 };

	MeshBuffer::Description cdesc = {};
	cdesc.pVtx = cvtx;
	cdesc.vtxSize = sizeof(Vertex);
	cdesc.vtxCount = _countof(cvtx);
	cdesc.pIdx = idx;
	cdesc.idxSize = sizeof(idx[0]);
	cdesc.idxCount = _countof(idx);
	cdesc.topology =
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_obj.push_back(new MeshBuffer(cdesc));
}
void SceneField::Uninit()
{
	std::vector<MeshBuffer*>::iterator it = m_obj.begin();
	while (it != m_obj.end())
	{
		delete (*it);
		++it;
	}
}
void SceneField::Update(float tick)
{
	m_time += tick;
}

void SceneField::Draw()
{
	// パラメータ取得
	CameraBase* pCamera = GetObj<CameraBase>("Camera");
	LightBase* pLight = GetObj<LightBase>("Light");
	RenderTarget* pRTV = GetObj<RenderTarget>("RTV");
	DepthStencil* pDSV = GetObj<DepthStencil>("DSV");

	// 姿勢行列の設定
	DirectX::XMFLOAT4X4 mat[3];
	DirectX::XMStoreFloat4x4(&mat[0], DirectX::XMMatrixIdentity());
	mat[1] = pCamera->GetView();
	mat[2] = pCamera->GetProj();
	// ライトの情報
	DirectX::XMFLOAT3 dir = pLight->GetDirection();
	DirectX::XMFLOAT4 light[3] = {
		pLight->GetDiffuse(),
		pLight->GetAmbient(),
		{dir.x, dir.y, dir.z, 0.0f}
	};
	// カメラ&時間情報
	DirectX::XMFLOAT3 camPos = pCamera->GetPos();
	DirectX::XMFLOAT4 camParam[] = {
		{camPos.x, camPos.y, camPos.z, m_time}
	};

	// シェーダー取得
	Shader* shader[] = {
		GetObj<Shader>("VS_Field"),
		GetObj<Shader>("PS_Field"),
		GetObj<Shader>("VS_Sea"),
		GetObj<Shader>("PS_Sea"),
		GetObj<Shader>("VS_WorldPostion"),
		GetObj<Shader>("PS_Atomosphere"),
		GetObj<Shader>("PS_Cloud")
	};

	//空
	SetRenderTargets(1, &pRTV, nullptr);
	DirectX::XMStoreFloat4x4(&mat[0],
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixTranslation(
				camPos.x, camPos.y, camPos.z)));
	shader[4]->WriteBuffer(0, mat);
	shader[5]->WriteBuffer(0, camParam);
	shader[4]->Bind();
	shader[5]->Bind();
	m_obj[1]->Draw();
	SetRenderTargets(1, &pRTV, pDSV);
	DirectX::XMStoreFloat4x4(&mat[0],
		DirectX::XMMatrixIdentity());

	// 地形
	shader[0]->WriteBuffer(0, mat);
	shader[1]->WriteBuffer(0, light);
	shader[0]->Bind();
	shader[1]->Bind();
	m_obj[0]->Draw();


	// 海
	shader[2]->WriteBuffer(0, mat);
	shader[2]->WriteBuffer(1, camParam);
	shader[3]->WriteBuffer(0, light);
	shader[3]->WriteBuffer(1, camParam);
	shader[3]->Bind();
	shader[2]->Bind();
	m_obj[0]->Draw();


	//雲
	DirectX::XMStoreFloat4x4(
		&mat[0],
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(30.f, 1.0f, 30.f) *
			DirectX::XMMatrixTranslation(0.0, 8.0f, 0.0f)));
	shader[4]->WriteBuffer(0, mat);
	shader[4]->Bind();
	shader[6]->WriteBuffer(0, camParam);
	shader[6]->WriteBuffer(1, light);
	shader[6]->Bind();

	m_obj[2]->Draw();
}