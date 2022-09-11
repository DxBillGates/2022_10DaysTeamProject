#include "Tutorial.h"
#include <GatesEngine/External/imgui/imgui.h>

const GE::Math::Vector3 Tutorial::POS_GENERATE_ENEMY_1 = { 1920 * 25 / 32, 1080 / 2 - 50, 0 };
const GE::Math::Vector3 Tutorial::POS_GENERATE_ENEMY_2 = { 1920 * 23 / 32, 1080 / 2 - 200, 0 };

const float Tutorial::FIRST_PLAYER_POS_X = 1920 / 2;

const float Tutorial::SECOND_SHADOW_POS = 1920 / 2;

const float Tutorial::THIRD_PLAYER_POS = 1920 * 20 / 32;

const float Tutorial::THIRD_SHADOW_POS = 1920 * 30 / 32;

const float Tutorial::FOURTH_PLAYER_POS_X1 = 1920 * 28 / 32;
const float Tutorial::FOURTH_PLAYER_POS_X2 = 1920 * 29 / 32;

const float Tutorial::FOURTH_SHADOW_POS = 1920 * 15 / 32;

const float Tutorial::PRE_START_TIME = 2.0f;

TutorialState Tutorial::tutorialState = TutorialState::GAME_START;
float Tutorial::tutorialTimer = 0;
int Tutorial::changeStateCount = 1;
bool Tutorial::attackable = false;

void Tutorial::Initialize(bool isSkipTutorial)
{
	tutorialTimer = 0;

	if (isSkipTutorial) {
		tutorialState = TutorialState::GAME_START;
		attackable = true;
	}
	else{
		tutorialState = TutorialState::FIRST_ATTACK;
		attackable = false;
	}
}

void Tutorial::UpdateTimer(float deltaTime)
{
	tutorialTimer += deltaTime;

	//チュートリアル終了処理
	if (tutorialState == TutorialState::PRE_START &&
		tutorialTimer >= PRE_START_TIME) {
		tutorialState = TutorialState::GAME_START;
	}

	ImGui::Begin("TutorialState");
	ImGui::Text("%d", (int)tutorialState);
	ImGui::End();
}

void Tutorial::DecrementChangeStateCount(int resetNum)
{
	changeStateCount--;

	//カウント0で状態遷移
	if (changeStateCount <= 0) {
		tutorialState = (TutorialState)((int)tutorialState + 1);

		//カウントとタイマーリセット
		changeStateCount = resetNum;
		tutorialTimer = 0;
	}
}