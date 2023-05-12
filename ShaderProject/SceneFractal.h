#ifndef __SCENE_FRACTAL_H__
#define __SCENE_FRACTAL_H__

#include "SceneBase.hpp"
#include "MeshBuffer.h"

class SceneFractal : public SceneBase
{
public:
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();

private:
	void DrawTriangle(int maxDepth, int depth, float x, float y);
	void DrawCube(int maxDepth, int depth, float x, float y, float z);

private:
	MeshBuffer* m_pTriangle;
	MeshBuffer* m_pCube;
};

#endif // __SCENE_FRACTAL_H__