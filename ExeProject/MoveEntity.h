#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Util/Math/Vector3.h>

// 自身の姿勢(頭がどっちか)
// normalなら頭は上、inverseなら下
enum class StanceState
{
	NORMAL,
	INVERSE,
};

enum class MoveDirectionState
{
	RIGHT,
	LEFT,
};

class MoveEntity
{
private:
	StanceState stanceState;
	MoveDirectionState moveDirectionState;

	GE::FlagController changeMoveDirectionFlag;
	GE::Math::Vector3 angles;
public:

	void Initialize();

	void UpdateStanceAngle(float deltaTime, float gameTime);
	void UpdateChangeDirectionFlag(float deltaTime,float gameTime);

	// 移動方向を逆に変更する
	void ChangeMoveDirection();

	StanceState GetStanceState()
	{
		return stanceState;
	}

	GE::Math::Vector3 GetAngles()
	{
		return angles;
	}

	void SetStanceState(StanceState state)
	{
		stanceState = state;
	}
};
