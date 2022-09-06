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
	/// SetTime�ŕύX�ɂ����鎞�Ԃ�ݒ肵���ې��`��ԂŎ��Ԃ�ύX����p
	/// </summary>
	/// <param name="deltaTime">1�t���[���̑��x</param>
	void ChangingGameTime(float deltaTime);

	float GetTime();

	/// <summary>
	/// �Q�[���̌o�ߑ��x��ύX����
	/// </summary>
	/// <param name="setTime">�ύX��̒l</param>
	/// <param name="changingTime">�ύX�܂łɂ����鎞��(s)</param>
	void SetTime(float setTime,float changingTime);

private:
	GameSetting();
	GameSetting(const GameSetting&) = delete;
	~GameSetting() = default;
	GameSetting& operator=(const GameSetting&) = delete;
};
