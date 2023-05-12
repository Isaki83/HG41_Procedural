#include "SceneFractal.h"
#include "Model.h"
#include "CameraBase.h"
#include "LightBase.h"
#include "Input.h"

using namespace DirectX;

void SceneFractal::Init()
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

	// ���_�o�b�t�@�̍쐬
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

	// �����̍쐬
	Vertex cvtx[] = {
		// ��̖�
		{{-0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},
		{{ 0.5f, 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},},
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},
		{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},},
		// ���̖�
		{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},},
		// ��O�̖�
		{{-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{ 0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		{{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},},
		// ���̖�
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},},
		// ���̖�
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},},
		// �E�̖�
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
	int depth = 3;
	//DrawTriangle(depth, depth, 0.0f, 0.0f);

	shader[3]->Bind();
	DrawCube(depth, depth, 0.0f, 0.0f, 0.0f);
}

void SceneFractal::DrawTriangle(int maxDepth, int depth, float x, float y)
{
	if (depth > 0) 
	{
		// �O�p�`�Ɍ����J���Ĉړ�
		/*
		   --- �O�p�`�Ɍ����J���邽�тɁA�ړ��ʂ�1/2�ɂȂ�
		   --- 1��1/4  2��1/8  3��1/16 ...
		*/
		float dist = 1.0f / powf(2.0f, 2.0f + (maxDepth - depth));
		// ��Ɉړ�
		DrawTriangle(maxDepth, depth - 1, x + 0.0f, y + dist);
		// �����Ɉړ�
		DrawTriangle(maxDepth, depth - 1, x - dist, y - dist);
		// �E���Ɉړ�
		DrawTriangle(maxDepth, depth - 1, x + dist, y - dist);
	}
	else 
	{
		// �`��
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
		// ���������鏈��
		// �����̂𕪊��������ꂼ��̉ӏ��ŕ\�����邩���Ȃ������`
		int idx[] = {
			1,1,1, 1,0,1, 1,1,1,	// ��i
			1,0,1, 0,0,0, 1,0,1,	// ���f
			1,1,1, 1,0,1, 1,1,1,	// ���i
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
		// �`��
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