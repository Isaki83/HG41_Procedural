#ifndef __SCENE_TEXTURE_H__
#define __SCENE_TEXTURE_H__

#include "SceneBase.hpp"

class SceneTexture : public SceneBase
{
public:
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
private:
	float m_time;
};

#endif // __SCENE_TEXTURE_H__