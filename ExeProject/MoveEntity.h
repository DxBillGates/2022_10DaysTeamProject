#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Util/Math/Vector3.h>

// ���g�̎p��(�����ǂ�����)
// normal�Ȃ瓪�͏�Ainverse�Ȃ牺
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

	// �ړ��������t�ɕύX����
	void ChangeMoveDirection();
	// �㉺�̏�Ԃ�ύX����
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
