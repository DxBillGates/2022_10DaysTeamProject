#include "EffectManager.h"

EffectManager* EffectManager::GetInstance()
{
	static EffectManager instance;
	return &instance;
}

void EffectManager::Initialize()
{
	for (auto& effect : effects)
	{
		effect.second->Initialize();
	}
}

void EffectManager::Update(float deltaTime)
{
	for (auto& effect : effects)
	{
		effect.second->Update(deltaTime);
	}
}

void EffectManager::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
	for (auto& effect : effects)
	{
		if (effect.second->IsActive() == false)continue;

		effect.second->Draw(graphicsDevice);
	}
}

void EffectManager::Active(const std::string& effectName)
{
	effects.at(effectName)->Active();
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
	for (auto& effect : effects)
	{
		delete effect.second;
	}
}
