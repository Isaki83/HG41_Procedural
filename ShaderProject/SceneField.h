#ifndef __SCENE_FIELD_H__
#define __SCENE_FIELD_H__

#include "SceneBase.hpp"
#include "MeshBuffer.h"
#include <vector>

class SceneField : public SceneBase
{
public:
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
private:
	std::vector<MeshBuffer*> m_obj;
	float m_time;
};

#endif // __SCENE_FIELD_H__