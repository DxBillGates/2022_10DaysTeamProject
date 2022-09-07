#include "PlayerAttackManager.h"
#include "GameSetting.h"
#include <GatesEngine/Header/Input/InputDevice.h>
#include <GatesEngine/External/imgui/imgui.h>

PlayerAttackManager* PlayerAttackManager::GetInstance()
{
	static PlayerAttackManager instance;
	return &instance;
}

void PlayerAttackManager::Initialize()
{
	attackState = PlayerAttackState::NONE;
	attackStateFlag.Initialize();
	coolTimeFlag.Initialize();
}

void PlayerAttackManager::Update(float deltaTime)
{
	TransitionAttackStateProcess();
	AttackProcess();
	CoolTimeProcess();

	//if (attackState != PlayerAttackState::NONE)
	//{
	//	ImGui::Begin("State");
	//	switch (attackState)
	//	{
	//	case PlayerAttackState::PRE:
	//		ImGui::Text("PRE");
	//		break;
	//	case PlayerAttackState::ACTIVE:
	//		ImGui::Text("ACTIVE");
	//		break;
	//	case PlayerAttackState::END:
	//		ImGui::Text("END");
	//		break;
	//	}		
	//	ImGui::Text("sec %3.3f", attackStateFlag.GetTime());
	//	ImGui::End();
	//}

	coolTimeFlag.Update(deltaTime);
	attackStateFlag.Update(deltaTime);
}

PlayerAttackState PlayerAttackManager::GetAttackState()
{
	return attackState;
}

void PlayerAttackManager::SetPlayer(GE::GameObject* pPlayer)
{
	player = pPlayer;
}

void PlayerAttackManager::SetShadowPlayer(GE::GameObject* pPlayer)
{
	shadowPlayer = pPlayer;
}

PlayerAttackManager::PlayerAttackManager()
	: player(nullptr)
	, shadowPlayer(nullptr)
	, attackState(PlayerAttackState::NONE)
	, attackStateFlag(GE::FlagController())
	, coolTimeFlag(GE::FlagController())
{
}

void PlayerAttackManager::TransitionAttackStateProcess()
{
	// äeèÛë‘ÇÃëJà⁄éûä‘(s)
	const float PRE_TIME = 1;
	const float ACTIVE_TIME = 1;
	const float END_TIME = 1;
	const float COOL_TIME = 5;

	// çUåÇíÜÇÃÉQÅ[ÉÄë¨ìx(s)
	const float SLOW_TIME = 0.05f;
	const float TRANSITION_TIME = 0.5f;

	// ÉLÅ[ÇâüÇµÇΩÇ∆Ç´Ç…ÉNÅ[ÉãÉ^ÉCÉÄÇ™èIóπÇµÇƒÇ¢ÇƒÇ©Ç¬çUåÇëJà⁄Ç™NoneÇ»ÇÁçUåÇÇäJén
	if (GE::InputDevice::GetInstance()->GetKeyboard()->CheckPressTrigger(GE::Keys::SPACE) && attackState == PlayerAttackState::NONE 
		&& coolTimeFlag.GetFlag() == false)
	{
		TransitionAttackState(PRE_TIME, PlayerAttackState::PRE);
		GameSetting::GetInstance()->SetTime(SLOW_TIME,TRANSITION_TIME);
	}

	// äeçUåÇèÛë‘Ç©ÇÁéüÇÃëJà⁄Ç…à⁄ÇÈèàóù
	if (attackStateFlag.GetOverTimeTrigger())
	{
		switch (attackState)
		{
		case PlayerAttackState::PRE:
			TransitionAttackState(ACTIVE_TIME, PlayerAttackState::ACTIVE);
			break;
		case PlayerAttackState::ACTIVE:
			TransitionAttackState(END_TIME, PlayerAttackState::END);
			break;
		case PlayerAttackState::END:
			attackState = PlayerAttackState::NONE;
			GameSetting::GetInstance()->SetTime(1, TRANSITION_TIME);

			coolTimeFlag.Initialize();
			coolTimeFlag.SetMaxTimeProperty(COOL_TIME);
			coolTimeFlag.SetFlag(true);

			break;
		}
	}
}

void PlayerAttackManager::TransitionAttackState(const float SET_TIME, PlayerAttackState nextState)
{
	attackState = nextState;
	attackStateFlag.Initialize();
	attackStateFlag.SetMaxTimeProperty(SET_TIME);
	attackStateFlag.SetFlag(true);
}

void PlayerAttackManager::AttackProcess()
{
	if (attackState == PlayerAttackState::NONE)return;

	switch (attackState)
	{
	// ÉvÉåÉCÉÑÅ[ÇêUìÆÇ≥ÇπÇÈ
	case PlayerAttackState::PRE:
		break;
	case PlayerAttackState::ACTIVE:
		break;
	case PlayerAttackState::END:
		break;
	}
}

void PlayerAttackManager::CoolTimeProcess()
{
	if (coolTimeFlag.GetFlag() == false)return;

	if (coolTimeFlag.GetOverTimeTrigger() == true)
	{
		coolTimeFlag.Initialize();
	}
}
