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
		std::string base;				// 変換前文字列
		std::string str = initValue;	// 変換後文字列
		// イテレーション回数分、変換処理を実行
		for (int i = 0; i < iteration; ++i)
		{
			base = str;		// 変換済みの文字列を再度変換
			str = "";		// 変換後の文字を格納できるよう初期化
			it = base.begin();
			while (it != base.end())
			{
				auto rule = m_rule.find(*it);	// 変換ルールがあるか探索
				if (rule != m_rule.end())
				{
					str += rule->second;
				}		// 変換後の文字を追加
				else { str += *it; }			// 変換ルールがないので、現在の文字をそのまま追加
				++it;
			}
		}
		// 変換後の文字に対応する処理を実行
		it = str.begin();
		while (it != str.end())
		{
			auto behavior = m_behavior.find(*it);
			if (behavior != m_behavior.end()) { behavior->second(arg); }
			++it;
		}
	}
	// 変換ルールの追加
	void AddRule(char key, const char* str) { m_rule.insert(std::pair<char, std::string>(key, str)); }
	// 実行処理の追加
	void AddBehavior(char key, std::function<void(void*)> func) { m_behavior.insert(std::pair<char, std::function<void(void*)>>(key, func)); }

private:
	std::map<char, std::string> m_rule;						// 変換ルール
	std::map<char, std::function<void(void*)>> m_behavior;	// 文字の動作
};

#endif // __SCENE_LSYSTEM_H__