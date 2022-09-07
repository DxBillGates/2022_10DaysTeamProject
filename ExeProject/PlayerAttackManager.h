#pragma once
#include "MoveEntity.h"
#include <GatesEngine/Header/GameFramework/GameObject/GameObject.h>

enum class PlayerAttackState
{
	NONE,
	PRE,
	ACTIVE,
	END,
};

struct PlayerInfo
{
	GE::GameObject* object;
	MoveEntity* moveEntity;
};

class PlayerAttackManager
{
private:
	PlayerInfo player;
	PlayerInfo shadowPlayer;

	PlayerAttackState attackState;
	GE::FlagController attackStateFlag;

	GE::FlagController coolTimeFlag;

	struct PlayerVibrationInfo
	{
		GE::Math::Vector3 playerOriginPosition;
		GE::Math::Vector3 shadowPlayerOriginPosition;

		GE::FlagController flag;

		// true : ランダムな方向に移動
		// false : もとの位置に移動
		bool vibrationState;
	};
	PlayerVibrationInfo vibrationInfo;

public:
	static PlayerAttackManager* GetInstance();

	void Initialize();
	void Update(float deltaTime);

	PlayerAttackState GetAttackState();
	void SetPlayer(GE::GameObject* pPlayer,MoveEntity* moveEntity);
	void SetShadowPlayer(GE::GameObject* pPlayer, MoveEntity* moveEntity);
private:
	PlayerAttackManager();
	PlayerAttackManager(const PlayerAttackManager&) = delete;
	~PlayerAttackManager() = default;
	PlayerAttackManager& operator=(const PlayerAttackManager&) = delete;

	// 攻撃状態のオンからオフまでの処理遷移
	void TransitionAttackStateProcess();
	void TransitionAttackState(const float SET_TIME, PlayerAttackState nextState);

	void AttackProcess();

	void CoolTimeProcess();

	void InitializeVibrationInfo();
};
