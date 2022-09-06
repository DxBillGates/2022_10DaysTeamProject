#include "GameSetting.h"
#include <GatesEngine/Header/Util/Math/Math.h>

GameSetting* GameSetting::GetInstance()
{
	static GameSetting instance;
	return &instance;
}

void GameSetting::Initialize()
{
	beforeGameTime = gameTime = afterGameTime = 1;
	changeGameTimeFlag.Initialize();
}

void GameSetting::ChangingGameTime(float deltaTime)
{
	if (changeGameTimeFlag.GetFlag() == false)return;

	// 終了トリガーを確認し、フラグ管理を終わらせる
	if (changeGameTimeFlag.GetOverTimeTrigger() == true)
	{
		changeGameTimeFlag.SetTime(1);
		changeGameTimeFlag.SetFlag(false);
	}

	float lerpTime = changeGameTimeFlag.GetTime() / changeGameTimeFlag.GetMaxTimeProperty();
	gameTime = GE::Math::Lerp(beforeGameTime, afterGameTime, lerpTime);

	changeGameTimeFlag.Update(deltaTime);
}

float GameSetting::GetTime()
{
	return gameTime;
}

void GameSetting::SetTime(float setTime, float changingTime)
{
	changeGameTimeFlag.Initialize();
	changeGameTimeFlag.SetFlag(true);
	changeGameTimeFlag.SetMaxTimeProperty(changingTime);

	beforeGameTime = gameTime;
	afterGameTime = setTime;
}

GameSetting::GameSetting()
	: beforeGameTime(1)
	, gameTime(1)
	, afterGameTime(1)
	, changeGameTimeFlag()
{
}
