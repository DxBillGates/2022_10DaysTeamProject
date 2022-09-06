#include "MoveEntity.h"
#include <GatesEngine/Header/Util/Math/Math.h>
#include <GatesEngine/Header/Graphics/Window.h>

MoveEntity::MoveEntity()
	: stanceState(StanceState::NORMAL)
	, moveDirectionState(MoveDirectionState::RIGHT)
	, changeMoveDirectionFlag(GE::FlagController())
	, angles()
{
}

void MoveEntity::Initialize()
{
	const float CHANGE_DIRECTION_TIME = 0.3f;

	changeMoveDirectionFlag.Initialize();
	changeMoveDirectionFlag.SetMaxTimeProperty(CHANGE_DIRECTION_TIME);

	stanceState = StanceState::NORMAL;
	moveDirectionState = MoveDirectionState::RIGHT;
}

void MoveEntity::CheckTeleport(GE::Math::Vector3& pos,const GE::Math::Vector3 size)
{
	GE::Math::Vector2 windowSize = GE::Window::GetWindowSize();

	// 上下のテレポート　ラムダ式
	auto TeleportY = [windowSize,&pos,size,this]()
	{
		if (stanceState == StanceState::NORMAL)
		{
			pos.y = size.y / 2;
		}
		else
		{
			pos.y = windowSize.y - size.y / 2;
		}
		ChangeStanceState();
	};

	// 上下のテレポート
	if (pos.x > windowSize.x)
	{
		pos.x = size.x / 2;
		TeleportY();
	}
	else if (pos.x < 0)
	{
		pos.x = windowSize.x - size.x / 2;
		TeleportY();
	}
}

void MoveEntity::UpdateStanceAngle(float deltaTime, float gameTime)
{
	if (stanceState == StanceState::NORMAL)
	{
		angles.x = 0;
	}
	else
	{
		angles.x = 180;
	}
}

void MoveEntity::UpdateChangeDirectionFlag(float deltaTime,float gameTime)
{
	if (changeMoveDirectionFlag.GetFlag() == false)return;

	if (changeMoveDirectionFlag.GetOverTimeTrigger())
	{
		changeMoveDirectionFlag.SetFlag(false);
		changeMoveDirectionFlag.SetTime(changeMoveDirectionFlag.GetMaxTimeProperty());
	}

	float startAngleValue;
	float endAngleValue;

	// 現在の向きにあったy軸の遷移角度を設定
	if (moveDirectionState == MoveDirectionState::RIGHT)
	{
		startAngleValue = 180;
		endAngleValue = 0;
	}
	else
	{
		startAngleValue = 0;
		endAngleValue = 180;
	}

	// フラグの経過時間を0 ~ 1に変換
	float lerpTime = (changeMoveDirectionFlag.GetTime() / changeMoveDirectionFlag.GetMaxTimeProperty());

	angles.y = GE::Math::Lerp(startAngleValue, endAngleValue, lerpTime * gameTime);

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

void MoveEntity::ChangeStanceState()
{
	if (stanceState == StanceState::NORMAL)
	{
		stanceState = StanceState::INVERSE;
	}
	else
	{
		stanceState = StanceState::NORMAL;
	}
}
