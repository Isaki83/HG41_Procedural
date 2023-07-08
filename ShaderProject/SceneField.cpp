#include "SceneField.h"
#include "Shader.h"
#include "CameraBase.h"
#include "LightBase.h"

void SceneField::Init()
{
	// �����l�ݒ�
	m_time = 0.0f;

	// �V�F�[�_�[�̓ǂݍ��ݏ���
	const char* shaderPath[] = {
		"Assets/Shader/VS_Field.cso",
		"Assets/Shader/PS_Field.cso",
		"Assets/Shader/VS_Sea.cso",
		"Assets/Shader/PS_Sea2.cso",
	};
	Shader* shader[] = {
		CreateObj<VertexShader>("VS_Field"),
		CreateObj<PixelShader>("PS_Field"),
		CreateObj<VertexShader>("VS_Sea"),
		CreateObj<PixelShader>("PS_Sea2"),
	};
	for (int i = 0; i < _countof(shaderPath); ++i)
		shader[i]->Load(shaderPath[i]);

	//--- ���ʍ쐬
	// ���ʒ�`
	struct Vertex
	{
		float pos[3];
		float normal[3];
		float uv[2];
	};
	const float maxSize = 20.0f;
	const int GridNum = 200;
	const float planeSpace = maxSize / (GridNum - 1);
	// ���_����
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
	// �C���f�b�N�X����
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
	// ���_�o�b�t�@�쐬
	MeshBuffer::Description desc = {};
	desc.pVtx = planeVtx.data();
	desc.vtxCount = planeVtx.size();
	desc.vtxSize = sizeof(Vertex);
	desc.pIdx = planeIdx.data();
	desc.idxCount = planeIdx.size();
	desc.idxSize = sizeof(DWORD);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_obj.push_back(new MeshBuffer(desc));
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
	// �p�����[�^�擾
	CameraBase* pCamera = GetObj<CameraBase>("Camera");
	LightBase* pLight = GetObj<LightBase>("Light");

	// �p���s��̐ݒ�
	DirectX::XMFLOAT4X4 mat[3];
	DirectX::XMStoreFloat4x4(&mat[0], DirectX::XMMatrixIdentity());
	mat[1] = pCamera->GetView();
	mat[2] = pCamera->GetProj();
	// ���C�g�̏��
	DirectX::XMFLOAT3 dir = pLight->GetDirection();
	DirectX::XMFLOAT4 light[3] = {
		pLight->GetDiffuse(),
		pLight->GetAmbient(),
		{dir.x, dir.y, dir.z, 0.0f}
	};
	// �J����&���ԏ��
	DirectX::XMFLOAT3 camPos = pCamera->GetPos();
	DirectX::XMFLOAT4 camParam[] = {
		{camPos.x, camPos.y, camPos.z, m_time}
	};

	// �V�F�[�_�[�擾
	Shader* shader[] = {
		GetObj<Shader>("VS_Field"),
		GetObj<Shader>("PS_Field"),
		GetObj<Shader>("VS_Sea"),
		GetObj<Shader>("PS_Sea2"),
	};
	// �n�`
	shader[0]->WriteBuffer(0, mat);
	shader[1]->WriteBuffer(0, light);
	shader[0]->Bind();
	shader[1]->Bind();
	m_obj[0]->Draw();
	// �C
	// ���ԏ��
	DirectX::XMFLOAT4 time[] = {
		{m_time, 0.0f, 0.0f, 0.0f}
	};
	shader[2]->WriteBuffer(0, mat);
	shader[2]->WriteBuffer(1, camParam);
	shader[3]->WriteBuffer(0, light);
	shader[3]->WriteBuffer(1, camParam);

	shader[2]->Bind();
	shader[3]->Bind();
	m_obj[0]->Draw();
}