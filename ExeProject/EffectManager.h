#pragma once
#include "Effect.h"

#include <map>

class EffectManager
{
private:
	std::map <std::string, Effect*> effects;
public:
	static EffectManager* GetInstance();

	void Initialize();
	void Update(float deltaTime);
	void Draw(GE::IGraphicsDeviceDx12* graphicsDevice);
	void Active(const std::string& effectName);

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

	effects.insert(std::make_pair(effectName, pEffect));

	return newEffect;
}
