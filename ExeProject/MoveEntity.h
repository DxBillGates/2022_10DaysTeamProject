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

	void CheckTeleport(GE::Math::Vector3& pos,const GE::Math::Vector3 size);
	void UpdateStanceAngle(float deltaTime, float gameTime);
	void UpdateChangeDirectionFlag(float deltaTime,float gameTime);

	// 移動方向を逆に変更する
	void ChangeMoveDirection();
	// 上下の状態を変更する
	void ChangeStanceState();

	StanceState GetStanceState()
	{
		return stanceState;
	}

	MoveDirectionState GetDirectionState()
	{
		return moveDirectionState;
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
