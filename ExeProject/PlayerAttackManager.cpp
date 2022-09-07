#include "PlayerAttackManager.h"
#include "GameSetting.h"
#include <GatesEngine/Header/Input/InputDevice.h>
#include <GatesEngine/External/imgui/imgui.h>
#include <GatesEngine/Header/Util/Random.h>

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
	float frameTime = 0.016f;
	coolTimeFlag.Update(frameTime);
	attackStateFlag.Update(frameTime);
	vibrationInfo.flag.Update(frameTime);
}

PlayerAttackState PlayerAttackManager::GetAttackState()
{
	return attackState;
}

void PlayerAttackManager::SetPlayer(GE::GameObject* pPlayer, MoveEntity* moveEntity)
{
	player.object = pPlayer;
	player.moveEntity = moveEntity;
}

void PlayerAttackManager::SetShadowPlayer(GE::GameObject* pPlayer, MoveEntity* moveEntity)
{
	shadowPlayer.object = pPlayer;
	shadowPlayer.moveEntity = moveEntity;
}

PlayerAttackManager::PlayerAttackManager()
	: player()
	, shadowPlayer()
	, attackState(PlayerAttackState::NONE)
	, attackStateFlag(GE::FlagController())
	, coolTimeFlag(GE::FlagController())
{
}

void PlayerAttackManager::TransitionAttackStateProcess()
{
	// �e��Ԃ̑J�ڎ���(s)
	const float PRE_TIME = 1;
	const float ACTIVE_TIME = 0.5f;
	const float END_TIME = 1;
	const float COOL_TIME = 5;

	// �U�����̃Q�[�����x(s)
	const float SLOW_TIME = 0.05f;
	const float TRANSITION_TIME = 0.5f;

	// �L�[���������Ƃ��ɃN�[���^�C�����I�����Ă��Ă��U���J�ڂ�None�Ȃ�U�����J�n
	if (GE::InputDevice::GetInstance()->GetKeyboard()->CheckPressTrigger(GE::Keys::SPACE) && attackState == PlayerAttackState::NONE
		&& coolTimeFlag.GetFlag() == false)
	{
		TransitionAttackState(PRE_TIME, PlayerAttackState::PRE);
		GameSetting::GetInstance()->SetTime(SLOW_TIME, TRANSITION_TIME);

		InitializeVibrationInfo();
	}

	// �e�U����Ԃ��玟�̑J�ڂɈڂ鏈��
	if (attackStateFlag.GetOverTimeTrigger())
	{
		switch (attackState)
		{
		case PlayerAttackState::PRE:
			TransitionAttackState(ACTIVE_TIME, PlayerAttackState::ACTIVE);
			break;
		case PlayerAttackState::ACTIVE:
			TransitionAttackState(END_TIME, PlayerAttackState::END);

			player.object->GetTransform()->position = vibrationInfo.shadowPlayerOriginPosition;
			shadowPlayer.object->GetTransform()->position = vibrationInfo.playerOriginPosition;

			player.moveEntity->SetStanceState(shadowPlayer.moveEntity->GetStanceState());
			shadowPlayer.moveEntity->SetStanceState(player.moveEntity->GetStanceState());

			shadowPlayer.moveEntity->SetDirectionState(player.moveEntity->GetDirectionState());

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

	float lerpTime = attackStateFlag.GetTime() / attackStateFlag.GetMaxTimeProperty();

	switch (attackState)
	{
		// �v���C���[��U��������
	case PlayerAttackState::PRE:
		// �����_���ړ��Ɩ߂�̓���ւ�
		if (vibrationInfo.flag.GetOverTimeTrigger())
		{
			vibrationInfo.flag.SetTime(0);
			vibrationInfo.vibrationState = !vibrationInfo.vibrationState;

			if (vibrationInfo.vibrationState)
			{
				GE::Math::Vector3 randomVector;
				for (int i = 0; i < 3; ++i)
				{
					randomVector.value[i] = GE::RandomMaker::GetFloat(-1, 1);
				}

				player.object->GetTransform()->position += randomVector;
				shadowPlayer.object->GetTransform()->position += randomVector;
			}
			else
			{
				player.object->GetTransform()->position = vibrationInfo.playerOriginPosition;
				shadowPlayer.object->GetTransform()->position = vibrationInfo.shadowPlayerOriginPosition;
			}
		}
		break;
	case PlayerAttackState::ACTIVE:
		player.object->GetTransform()->position = GE::Math::Vector3::Lerp(vibrationInfo.playerOriginPosition, vibrationInfo.shadowPlayerOriginPosition, lerpTime);
		shadowPlayer.object->GetTransform()->position = GE::Math::Vector3::Lerp(vibrationInfo.shadowPlayerOriginPosition, vibrationInfo.playerOriginPosition, lerpTime);
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

void PlayerAttackManager::InitializeVibrationInfo()
{
	// �U���ԃt���[��(���t���[�����ƂɐU�������邩)
	const int VIBRATION_FRAME_COUNT = 5;

	vibrationInfo.playerOriginPosition = player.object->GetTransform()->position;
	vibrationInfo.shadowPlayerOriginPosition = shadowPlayer.object->GetTransform()->position;
	vibrationInfo.flag.Initialize();
	vibrationInfo.flag.SetMaxTimeProperty(1.0f / GameSetting::GetInstance()->GetMaxFramerate() * VIBRATION_FRAME_COUNT);
	vibrationInfo.flag.SetFlag(true);
}
