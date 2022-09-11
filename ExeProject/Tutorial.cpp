#include "Tutorial.h"
#include "PlayerAttackManager.h"
#include <GatesEngine/External/imgui/imgui.h>
#include <GatesEngine/Header/Graphics/Window.h>

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

GE::IGraphicsDeviceDx12* Tutorial::graphicsDevice = nullptr;

const GE::Math::Vector3 SCALE_TUTORIAL = { 384, 96, 0 };
const GE::Math::Vector3 SCALE_LEFT = { 512,256,0 };
const GE::Math::Vector3 SCALE_RIGHT = { 512,256,0 };
const GE::Math::Vector3 SCALE_ATTACK = { 512,256,0 };
const GE::Math::Vector3 SCALE_WAIT = { 192, 64, 0 };
const GE::Math::Vector3 SCALE_GRID = { 132, 147, 0 };

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
}

void Tutorial::Draw()
{
	const GE::Math::Vector3 POS_INSTRUCTIONS = {256, GE::Window::GetWindowSize().y / 2, 0};
	bool isAttacking = PlayerAttackManager::GetInstance()->GetAttackState() != PlayerAttackState::NONE;

	if (tutorialState == TutorialState::FIRST_ATTACK && isAttacking == false) {
		if (IsAttackable() == false) {
			//Left
			Draw(POS_INSTRUCTIONS, SCALE_LEFT, "TTR_Left");

			//Grid
			const GE::Math::Vector3 POS_GRID = { 885 + SCALE_GRID.x / 2,933 + SCALE_GRID.y / 2,0 };
			Draw(POS_GRID, SCALE_GRID, "TTR_Grid");
		}
		else {
			//Attack
			Draw(POS_INSTRUCTIONS, SCALE_ATTACK, "TTR_Attack");
		}
	}
	else if (tutorialState == TutorialState::SECOND_ATTACK && isAttacking == false) {
		if (IsAttackable() == false) {
			//Wait
			Draw(POS_INSTRUCTIONS, SCALE_WAIT, "TTR_Wait");
		}
		else {
			//Attack
			Draw(POS_INSTRUCTIONS, SCALE_ATTACK, "TTR_Attack");
		}
	}
	else if (tutorialState == TutorialState::THIRD_ATTACK && isAttacking == false) {
		if (IsAttackable() == false) {
			//Right
			Draw(POS_INSTRUCTIONS, SCALE_RIGHT, "TTR_Right");

			//Grid
			const GE::Math::Vector3 POS_GRID = { 1141 + SCALE_GRID.x / 2,SCALE_GRID.y / 2,0 };
			Draw(POS_GRID, SCALE_GRID, "TTR_Grid");
		}
		else {
			//Attack
			Draw(POS_INSTRUCTIONS, SCALE_ATTACK, "TTR_Attack");
		}
	}
	else if (tutorialState == TutorialState::FOURTH_ATTACK && isAttacking == false) {
		if (IsAttackable() == false) {
			//Left, Right
			Draw(GE::Math::Vector3(POS_INSTRUCTIONS.x / 2, POS_INSTRUCTIONS.y, 0), SCALE_LEFT / 2, "TTR_Left");
			Draw(GE::Math::Vector3(POS_INSTRUCTIONS.x * 3 / 2, POS_INSTRUCTIONS.y, 0), SCALE_RIGHT / 2, "TTR_Right");

			//Grid
			const GE::Math::Vector3 POS_GRID = { 1661 + SCALE_GRID.x / 2,SCALE_GRID.y / 2,0 };
			Draw(POS_GRID, SCALE_GRID, "TTR_Grid");
		}
		else {
			//Attack
			Draw(POS_INSTRUCTIONS, SCALE_ATTACK, "TTR_Attack");
		}
	}
	else {
		//ゲームスタート！みたいなのいれてもいいかも
	}
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

void Tutorial::Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(pos);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get(name)->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}
