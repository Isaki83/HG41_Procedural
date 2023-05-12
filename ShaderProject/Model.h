#ifndef __MODEL_H__
#define __MODEL_H__

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "Shader.h"
#include "MeshBuffer.h"
#include "Texture.h"

class Model
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};
	using Vertices = std::vector<Vertex>;
	using Indices = std::vector<unsigned int>;
	struct Material
	{
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 specular;
		std::shared_ptr<Texture> texture;
	};
	using MaterialPtr = std::shared_ptr<Material>;
	using Materials = std::vector<MaterialPtr>;
	struct Mesh
	{
		Vertices vtx;
		Indices idx;
		unsigned int materialID;
		std::shared_ptr<MeshBuffer> mesh;
	};
	using MeshPtr = std::shared_ptr<Mesh>;
	using Meshes = std::vector<MeshPtr>;

public:
	Model();
	~Model();
	void SetVertexShader(Shader* vs);
	void SetPixelShader(Shader* ps);
	const Mesh* GetMesh(unsigned int index);
	unsigned int GetMeshNum();

public:
	bool Load(const char* file, float scale = 1.0f, bool flip = false);
	void Draw(int texSlot = 0);

private:
	void MakeDefaultShader();

private:
	static std::shared_ptr<VertexShader> m_defVS;
	static std::shared_ptr<PixelShader> m_defPS;
private:
	Meshes m_meshes;
	Materials m_materials;
	VertexShader* m_pVS;
	PixelShader* m_pPS;
};


#endif // __MODEL_H__