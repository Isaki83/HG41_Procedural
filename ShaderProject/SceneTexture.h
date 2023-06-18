#ifndef __SCENE_TEXTURE_H__
#define __SCENE_TEXTURE_H__

#include "SceneBase.hpp"
#include "MeshBuffer.h"
#include "DirectXMath.h"

using namespace DirectX;

class SceneTexture : public SceneBase
{
public:
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
private:
	float m_time;

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
	};
	MeshBuffer* m_pCube;
	Vertex m_vtx[24];
};

#endif // __SCENE_TEXTURE_H__