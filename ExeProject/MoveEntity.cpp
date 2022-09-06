#include "MoveEntity.h"
#include <GatesEngine/Header/Util/Math/Math.h>

void MoveEntity::Initialize()
{
	const float CHANGE_DIRECTION_TIME = 0.5f;

	changeMoveDirectionFlag.Initialize();
	changeMoveDirectionFlag.SetMaxTimeProperty(CHANGE_DIRECTION_TIME);

	stanceState = StanceState::NORMAL;
	moveDirectionState = MoveDirectionState::RIGHT;
}

void MoveEntity::UpdateStanceAngle(float deltaTime, float gameTime)
{
	if (stanceState == StanceState::NORMAL)
	{
		angles.z = 0;
	}
	else
	{
		angles.z = 180;
	}
}

void MoveEntity::UpdateChangeDirectionFlag(float deltaTime,float gameTime)
{
	if (changeMoveDirectionFlag.GetFlag() == false)return;

	float startAngleValue;
	float endAngleValue;

	// ���݂̌����ɂ�����y���̑J�ڊp�x��ݒ�
	if (moveDirectionState == MoveDirectionState::RIGHT)
	{
		startAngleValue = 0;
		endAngleValue = 180;
	}
	else
	{
		startAngleValue = 180;
		endAngleValue = 0;
	}

	// �t���O�̌o�ߎ��Ԃ�0 ~ 1�ɕϊ�
	float lerpTime = (changeMoveDirectionFlag.GetTime() / changeMoveDirectionFlag.GetMaxTimeProperty()) * gameTime;
	angles.y = GE::Math::Lerp(startAngleValue, endAngleValue, lerpTime);

	changeMoveDirectionFlag.Update(deltaTime);
}

void MoveEntity::ChangeMoveDirection()
{
	if (moveDirectionState == MoveDirectionState::RIGHT)
	{
		moveDirectionState = MoveDirectionState::LEFT;
	}
	else
	{
		moveDirectionState = MoveDirectionState::RIGHT;
	}

	changeMoveDirectionFlag.SetFlag(true);
	changeMoveDirectionFlag.SetTime(0);
}
