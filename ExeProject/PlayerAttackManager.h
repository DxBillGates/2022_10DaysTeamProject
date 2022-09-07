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

	// UŒ‚ó‘Ô‚ÌƒIƒ“‚©‚çƒIƒt‚Ü‚Å‚Ìˆ—‘JˆÚ
	void TransitionAttackStateProcess();
	void TransitionAttackState(const float SET_TIME, PlayerAttackState nextState);

	void AttackProcess();

	void CoolTimeProcess();
};
