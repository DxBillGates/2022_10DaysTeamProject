#include "ShadowPlayerComponent.h"
#include "GameSetting.h"
#include "Tutorial.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

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

	const float SPRITE_SIZE = 100;
	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2,transform->scale.y / 2,0 };

}

void ShadowPlayerComponent::Update(float deltaTime)
{
	const float GAME_TIME = GameSetting::GetInstance()->GetTime();
	const float MOVE_SPEED = 7;

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
	}

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

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(moveEntity.GetAngles());
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color(0.5f, 0.5f, 0.5f, 1);
	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("texture_player")->GetSRVNumber() });
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
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//2秒後以降、下側にいて、画面の左半分に来た時に停止
		return !(Tutorial::GetTutorialTimer() >= 2 &&
			moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x < Tutorial::SECOND_SHADOW_POS);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//下側にいて、画面の指定位置に来た時に停止
		return !(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x < Tutorial::THIRD_SHADOW_POS);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//2秒後以降、上側にいて、画面の指定位置に来た時に停止
		return !(Tutorial::GetTutorialTimer() >= 2 &&
			moveEntity.GetStanceState() == StanceState::INVERSE &&
			transform->position.x > Tutorial::FOURTH_SHADOW_POS);
	}
	else {
		return true;
	}
}

void ShadowPlayerComponent::UpdateAttackable()
{
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//Player側で管理するのでなにもしない
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//下側にいて、画面の左半分に来た時
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::NORMAL && transform->position.x <= Tutorial::FIRST_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//Player側で管理するのでなにもしない
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//Player側で管理するのでなにもしない
	}
}
