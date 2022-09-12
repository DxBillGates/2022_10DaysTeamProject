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
		for (auto& e : effect.second)
		{
			e->Initialize();
		}
	}
}

void EffectManager::Update(float deltaTime)
{
	for (auto& effect : effects)
	{
		for (auto& e : effect.second)
		{
			e->Update(deltaTime);
		}
	}
}

void EffectManager::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
	for (auto& effect : effects)
	{
		for (auto& e : effect.second)
		{
			if (e->IsActive() == false) return;

			e->Draw(graphicsDevice);
		}
	}
}

void EffectManager::Active(const std::string& effectName, const GE::Math::Vector3& position)
{
	//effects.at(effectName)->Active();
	for (auto& effect : effects)
	{
		for (auto& e : effect.second)
		{
			if (e->IsActive() == true) continue;

			e->Active(position);
			break;
		}
	}
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
	for (auto& effect : effects)
	{
		for (auto& e : effect.second)
		{
			delete e;
		}
	}
}
