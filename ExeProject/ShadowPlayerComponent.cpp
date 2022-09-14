#include "ShadowPlayerComponent.h"
#include "GameSetting.h"
#include "Camera2D.h"
#include "Tutorial.h"
#include "GameUtility.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

const int ShadowPlayerComponent::MAX_ANIMATION_NUMBER = 4;
const float ShadowPlayerComponent::CHANGE_ANIMATION_TIME_WALK = 0.25f / 2;
const float ShadowPlayerComponent::CHANGE_ANIMATION_TIME_STOP = 0.25f;
const GE::Math::Vector2 ShadowPlayerComponent::TEXTURE_SIZE = { 384,96 };
const GE::Math::Vector2 ShadowPlayerComponent::CLIP_SIZE = { 96 };

ShadowPlayerComponent::ShadowPlayerComponent()
	: moveEntity(MoveEntity())
	, autoMove(true)
{
}

void ShadowPlayerComponent::Start()
{
	moveEntity.Initialize();
	moveEntity.SetStanceState(StanceState::INVERSE);

	autoMove = true;

	const float SPRITE_SIZE = 96;
	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 * 3 / 8, transform->scale.y / 2,0 };

}

void ShadowPlayerComponent::Update(float deltaTime)
{
	const float GAME_TIME = GameSetting::GetInstance()->GetTime();
	const float MOVE_SPEED = 12;

	bool isMove = false;
	if (autoMove == true && ChackMovable() == true)
	{
		if (moveEntity.GetDirectionState() == MoveDirectionState::RIGHT)
		{
			transform->position.x += MOVE_SPEED * GAME_TIME;
		}
		else
		{
			transform->position.x -= MOVE_SPEED * GAME_TIME;
		}

		isMove = true;
	}

	const float MAX_ANIMATION_TIME = isMove ? CHANGE_ANIMATION_TIME_WALK : CHANGE_ANIMATION_TIME_STOP;
	if (drawAnimationTimer >= MAX_ANIMATION_TIME)
	{
		drawAnimationTimer = 0;
		++drawAnimationNumber;

		if (drawAnimationNumber >= MAX_ANIMATION_NUMBER)
		{
			drawAnimationNumber = 0;
		}
	}

	drawAnimationTimer += deltaTime;

	// 移動オブジェクト用の各種更新処理
	moveEntity.CheckTeleport(transform->position, transform->scale);
	moveEntity.UpdateChangeDirectionFlag(deltaTime, 1);
	moveEntity.UpdateStanceAngle(deltaTime, 1);

	//チュートリアル時攻撃可能かチェック
	UpdateAttackable();
}

void ShadowPlayerComponent::LateDraw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteTextureAnimationShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(moveEntity.GetAngles());
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color(1);
	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &Camera2D::GetInstance()->GetCameraInfo(), sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("Player_Shadow")->GetSRVNumber() });

	GE::TextureAnimationInfo animationInfo;
	animationInfo.clipSize = CLIP_SIZE;
	animationInfo.pivot = { (float)drawAnimationNumber,0 };
	animationInfo.textureSize = TEXTURE_SIZE;
	renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

	graphicsDevice->DrawMesh("2DPlane");
}

void ShadowPlayerComponent::OnGui()
{
	ImGui::Checkbox("flag", &autoMove);
}

MoveEntity* ShadowPlayerComponent::GetMoveEntity()
{
	return &moveEntity;
}

bool ShadowPlayerComponent::ChackMovable()
{
	//チュートリアル状態によって動き方を変える
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		return false;
		////上側にいて、画面の指定位置より右に来た時に停止
		//return !(moveEntity.GetStanceState() == StanceState::INVERSE &&
		//	transform->position.x > Tutorial::FIRST_SHADOW_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//下側にいて、画面の指定位置より左に来た時に停止
		return !(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x < Tutorial::SECOND_SHADOW_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//上側にいて、画面の指定位置より右に来た時に停止
		return !(moveEntity.GetStanceState() == StanceState::INVERSE &&
			transform->position.x > Tutorial::THIRD_SHADOW_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//下側にいて、画面の指定位置より右に来た時に停止
		return !(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x > Tutorial::FOURTH_SHADOW_POS_X);
	}
	else {
		return GameUtility::GetGameState() == GameState::GAME;
	}
}

void ShadowPlayerComponent::UpdateAttackable()
{
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//上側にいて、指定の位置よち右に来た時
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::INVERSE && transform->position.x >= Tutorial::FIRST_SHADOW_POS_X, 1);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//下側にいて、指定の位置より左に来た時
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::NORMAL && transform->position.x <= Tutorial::SECOND_SHADOW_POS_X, 1);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//上側にいて、指定の位置よち右に来た時
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::INVERSE && transform->position.x >= Tutorial::THIRD_SHADOW_POS_X, 1);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//下側にいて、指定の位置よち右に来た時
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::NORMAL && transform->position.x >= Tutorial::FOURTH_SHADOW_POS_X, 1);
	}
}
