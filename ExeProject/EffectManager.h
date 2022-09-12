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

// Effect�N���X���p�����Ă��邱�Ƃ��m�F������T�̃C���X�^���X���쐬
template<typename T>
inline T* EffectManager::Add(const std::string& effectName)
{
	T* newEffect = new T();
	Effect* pEffect = static_cast<Effect*>(newEffect);

	// �p�����Ă��Ȃ��Ȃ琶�������C���X�^���X���폜
	if (pEffect == nullptr)
	{
		delete newEffect;
		return nullptr;
	}

	effects.insert(std::make_pair(effectName, pEffect));

	return newEffect;
}
