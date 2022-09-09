#include "HitStopManager.h"
#include "GameSetting.h"

HitStopManager* HitStopManager::GetInstance()
{
	static HitStopManager instance;
	return &instance;
}

void HitStopManager::Initialize()
{
	flag.Initialize();
}

void HitStopManager::Update(float deltaTime)
{
	if (flag.GetOverTimeTrigger())
	{
		GameSetting* gameSetting = GameSetting::GetInstance();
		flag.Initialize();
		gameSetting->SetTime(1, 0.05f);
	}

	flag.Update(deltaTime);
}

void HitStopManager::Active(float time)
{
	flag.Initialize();
	flag.SetMaxTimeProperty(time);
	flag.SetFlag(true);

	GameSetting* gameSetting = GameSetting::GetInstance();
	gameSetting->SetTime(0, 0.05f);
}

bool HitStopManager::IsActive()
{
	return flag.GetFlag();
}

HitStopManager::HitStopManager()
	: flag()
{
}
