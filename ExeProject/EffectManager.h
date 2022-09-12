#pragma once
#include "Effect.h"

#include <map>
#include <unordered_map>

class EffectManager
{
private:
	std::map <std::string, std::vector<Effect*>> effects;
public:
	static EffectManager* GetInstance();

	void Initialize();
	void Update(float deltaTime);
	void Draw(GE::IGraphicsDeviceDx12* graphicsDevice);
	void Active(const std::string& effectName, const GE::Math::Vector3& position,float scale = 1);

	template<typename T>
	T* Add(const std::string& effectName);
private:
	EffectManager();
	EffectManager(const EffectManager&) = delete;
	~EffectManager();
	EffectManager& operator=(const EffectManager&) = delete;
};

// Effectクラスを継承していることを確認した後Tのインスタンスを作成
template<typename T>
inline T* EffectManager::Add(const std::string& effectName)
{
	T* newEffect = new T();
	Effect* pEffect = static_cast<Effect*>(newEffect);

	// 継承していないなら生成したインスタンスを削除
	if (pEffect == nullptr)
	{
		delete newEffect;
		return nullptr;
	}

	// すでにそのキーの要素がある場合
	decltype(effects)::iterator it = effects.find(effectName);
	if (it != effects.end())
	{
		it->second.push_back(pEffect);
		return newEffect;
	}

	effects.insert(std::make_pair(effectName, std::vector<Effect*>()));
	effects.at(effectName).push_back(pEffect);

	return newEffect;
}
