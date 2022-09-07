#pragma once
#include "PlayerComponent.h"
#include "ShadowPlayerComponent.h"

enum class PlayerAttackState
{
	NONE,
	PRE,
	ACTIVE,
	END,
};

class PlayerAttackManager
{
private:
	GE::GameObject* player;
	GE::GameObject* shadowPlayer;

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
	void SetPlayer(GE::GameObject* pPlayer);
	void SetShadowPlayer(GE::GameObject* pPlayer);
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
