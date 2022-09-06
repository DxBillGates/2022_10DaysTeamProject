#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>

class GameSetting
{
private:
	float beforeGameTime;
	float gameTime;
	float afterGameTime;

	GE::FlagController changeGameTimeFlag;
public:
	static GameSetting* GetInstance();

	void Initialize();
	
	/// <summary>
	/// SetTimeで変更にかかる時間を設定した際線形補間で時間を変更する用
	/// </summary>
	/// <param name="deltaTime">1フレームの速度</param>
	void ChangingGameTime(float deltaTime);

	float GetTime();

	/// <summary>
	/// ゲームの経過速度を変更する
	/// </summary>
	/// <param name="setTime">変更後の値</param>
	/// <param name="changingTime">変更までにかかる時間(s)</param>
	void SetTime(float setTime,float changingTime);

private:
	GameSetting();
	GameSetting(const GameSetting&) = delete;
	~GameSetting() = default;
	GameSetting& operator=(const GameSetting&) = delete;
};
