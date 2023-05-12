#include "SceneLSystem.h"
#include "Model.h"
#include "CameraBase.h"
#include "LightBase.h"
#include <stack>

using namespace DirectX;

void SceneLSystem::Init()
{
	// �V�F�[�_�[�̓ǂݍ��ݏ���
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

	// ���_���W
	struct Vertex { float x, y, z; };
	std::vector<Vertex> vtx;
	// �X�^�b�N���
	struct Param
	{
		XMFLOAT3 pos;	// ���݂̈ʒu
		XMFLOAT3 vec;	// ���݂̐i�s����
	};
	std::stack<Param> stack;
	stack.push( { XMFLOAT3(0,0,0), XMFLOAT3(0, 1, 0) } );

	// --- LSystem�𗘗p���āA���̂悤�Ȑ}�`�𒼐��\������
	LSystem lsystem;
	// ���[���ǉ�
	lsystem.AddRule('F', "X[+F][-F]");
	// �ړ�����
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
	// �X�^�b�N����
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

	// ~~~ �ꕔ���[��(+,- �̉�]����)�͏ȗ� ~~~

	
	// �\�z�ς݂̃��[���A�����Ɋ�Â���LSystem���Œ��_�f�[�^�𐶐�
	lsystem.Excute(6, "F", &stack);

	// ���_�o�b�t�@�𐶐�
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
	// GetObj �����Ŏw�肳�ꂽ���O�̃f�[�^��
	//        �e���v���[�g�����Ɏw�肳�ꂽ�^�ɕϊ����Ď擾
	CameraBase* pCamera = GetObj<CameraBase>("Camera");
	LightBase* pLight = GetObj<LightBase>("Light");
	// �V�F�[�_�[�擾
	Shader* shader[] = {
		GetObj<VertexShader>("VS_Object"),
		GetObj<PixelShader>("PS_Color"),
		GetObj<PixelShader>("PS_TexColor"),
		GetObj<PixelShader>("PS_Lambert"),
	};

	// �p���s��̏��
	XMFLOAT4X4 mat[3];
	XMStoreFloat4x4(&mat[0], XMMatrixIdentity());
	mat[1] = pCamera->GetView();
	mat[2] = pCamera->GetProj();
	// �J�����̏��
	XMFLOAT3 dir = pLight->GetDirection();
	XMFLOAT4 light[3] = {
		pLight->GetDiffuse(),
		pLight->GetAmbient(),
		{dir.x, dir.y, dir.z, 0.0f}
	};

	// �f�[�^��������
	shader[0]->WriteBuffer(0, mat);
	shader[3]->WriteBuffer(0, light);

	// �`��
	shader[0]->Bind();
	shader[1]->Bind();
	m_pTree->Draw();
}