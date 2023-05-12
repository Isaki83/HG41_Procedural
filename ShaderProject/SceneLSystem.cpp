#include "SceneLSystem.h"
#include "Model.h"
#include "CameraBase.h"
#include "LightBase.h"
#include <stack>

using namespace DirectX;

void SceneLSystem::Init()
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

	// 頂点座標
	struct Vertex { float x, y, z; };
	std::vector<Vertex> vtx;
	// スタック情報
	struct Param
	{
		XMFLOAT3 pos;	// 現在の位置
		XMFLOAT3 vec;	// 現在の進行方向
	};
	std::stack<Param> stack;
	stack.push( { XMFLOAT3(0,0,0), XMFLOAT3(0, 1, 0) } );

	// --- LSystemを利用して、作例のような図形を直線表示する
	LSystem lsystem;
	// ルール追加
	lsystem.AddRule('F', "X[+F][-F]");
	// 移動処理
	auto moveFunc = [&vtx](void* arg)
	{
		std::stack<Param>* pStack = reinterpret_cast<std::stack<Param>*>(arg);
		Param& top = pStack->top();
		vtx.push_back({ top.pos.x, top.pos.y, top.pos.z });
		top.pos.x += top.vec.x;
		top.pos.y += top.vec.y;
		top.pos.z += top.vec.z;
		vtx.push_back({ top.pos.x, top.pos.y, top.pos.z });
	};
	lsystem.AddBehavior('F', moveFunc);
	lsystem.AddBehavior('X', moveFunc);
	// スタック処理
	lsystem.AddBehavior('[',
		[&vtx](void* arg) {
		std::stack<Param>* pStack = reinterpret_cast<std::stack<Param>*>(arg);
		pStack->push(pStack->top());
	});
	lsystem.AddBehavior(']',
		[&vtx](void* arg) {
		std::stack<Param>* pStack = reinterpret_cast<std::stack<Param>*>(arg);
		pStack->pop();
	});

	// ~~~ 一部ルール(+,- の回転処理)は省略 ~~~

	
	// 構築済みのルール、処理に基づいてLSystem内で頂点データを生成
	lsystem.Excute(6, "F", &stack);

	// 頂点バッファを生成
	MeshBuffer::Description desc = {};
	desc.vtxSize = sizeof(Vertex);
	desc.vtxCount = vtx.size();
	desc.pVtx = vtx.data();
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_pTree = new MeshBuffer(desc);
}
void SceneLSystem::Uninit()
{
	delete m_pTree;
}
void SceneLSystem::Update(float tick)
{

}
void SceneLSystem::Draw()
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
	m_pTree->Draw();
}