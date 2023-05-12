#ifndef __SCENE_LSYSTEM_H__
#define __SCENE_LSYSTEM_H__

#include "SceneBase.hpp"
#include "MeshBuffer.h"
#include <functional>

class SceneLSystem : public SceneBase
{
public:
	void Init();
	void Uninit();
	void Update(float tick);
	void Draw();
	
private:
	MeshBuffer* m_pTree;
};

class LSystem
{
public:
	LSystem() {}
	~LSystem() {}
	void Excute(int iteration, const char* initValue, void* arg = nullptr)
	{
		std::string::iterator it;
		std::string base;				// �ϊ��O������
		std::string str = initValue;	// �ϊ��㕶����
		// �C�e���[�V�����񐔕��A�ϊ����������s
		for (int i = 0; i < iteration; ++i)
		{
			base = str;		// �ϊ��ς݂̕�������ēx�ϊ�
			str = "";		// �ϊ���̕������i�[�ł���悤������
			it = base.begin();
			while (it != base.end())
			{
				auto rule = m_rule.find(*it);	// �ϊ����[�������邩�T��
				if (rule != m_rule.end())
				{
					str += rule->second;
				}		// �ϊ���̕�����ǉ�
				else { str += *it; }			// �ϊ����[�����Ȃ��̂ŁA���݂̕��������̂܂ܒǉ�
				++it;
			}
		}
		// �ϊ���̕����ɑΉ����鏈�������s
		it = str.begin();
		while (it != str.end())
		{
			auto behavior = m_behavior.find(*it);
			if (behavior != m_behavior.end()) { behavior->second(arg); }
			++it;
		}
	}
	// �ϊ����[���̒ǉ�
	void AddRule(char key, const char* str) { m_rule.insert(std::pair<char, std::string>(key, str)); }
	// ���s�����̒ǉ�
	void AddBehavior(char key, std::function<void(void*)> func) { m_behavior.insert(std::pair<char, std::function<void(void*)>>(key, func)); }

private:
	std::map<char, std::string> m_rule;						// �ϊ����[��
	std::map<char, std::function<void(void*)>> m_behavior;	// �����̓���
};

#endif // __SCENE_LSYSTEM_H__