#include "PlayerComponent.h"
#include "GameSetting.h"
#include "HitStopManager.h"
#include "Tutorial.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

PlayerComponent::PlayerComponent()
	: inputDevice(nullptr)
	, moveEntity(MoveEntity())
{
}

void PlayerComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	moveEntity.Initialize();

	const float SPRITE_SIZE = 100;
	transform->scale = SPRITE_SIZE;

	transform->position = { 1920 * 7 / 8, GE::Window::GetWindowSize().y - transform->scale.y / 2,0 };
}

void PlayerComponent::Update(float deltaTime)
{
	const float GAME_TIME = GameSetting::GetInstance()->GetTime();

	//チュートリアルなどで移動不可状態かチェック
	if (CheckMovable()) {

		// 移動方向の変更テスト
		if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A) && moveEntity.GetDirectionState() == MoveDirectionState::RIGHT
			|| inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D) && moveEntity.GetDirectionState() == MoveDirectionState::LEFT)
		{
			moveEntity.ChangeMoveDirection();
		}

		const float MOVE_SPEED = 5;
		if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A))
		{
			transform->position.x -= MOVE_SPEED * GAME_TIME;
		}
		if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D))
		{
			transform->position.x += MOVE_SPEED * GAME_TIME;
		}
	}

	transform->position += knockbackVelocity * GAME_TIME;

	// 移動オブジェクト用の各種更新処理
	moveEntity.CheckTeleport(transform->position, transform->scale);
	moveEntity.UpdateChangeDirectionFlag(deltaTime, GAME_TIME);
	moveEntity.UpdateStanceAngle(deltaTime, GAME_TIME);


	if (invincibleFlag.GetOverTimeTrigger())
	{
		invincibleFlag.Initialize();
	}
	invincibleFlag.Update(deltaTime * GAME_TIME);

	UpdateKnockback(deltaTime * GAME_TIME);

	//チュートリアル時攻撃可能かチェック
	UpdateAttackable();
}

void PlayerComponent::LateDraw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(moveEntity.GetAngles());
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("texture_player")->GetSRVNumber() });

	//GE::TextureAnimationInfo animationInfo;
	//animationInfo.clipSize = 64;
	//animationInfo.pivot = { 1,1 };
	//renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

	graphicsDevice->DrawMesh("2DPlane");
}

void PlayerComponent::OnCollision(GE::GameObject* other)
{
	Knockback(other->GetTransform()->position);
}

MoveEntity* PlayerComponent::GetMoveEntity()
{
	return &moveEntity;
}

void PlayerComponent::Knockback(const GE::Math::Vector3& otherPosition)
{
	if (invincibleFlag.GetFlag() == true)return;

	const float POWER = 3;
	const float KNOCKBACK_TIME = 0.5f;
	const float INVINCIBLE_TIME = 1.0f;

	setKnockbackVector = knockbackVelocity = GE::Math::Vector3::Normalize(transform->position - otherPosition) * POWER;
	setKnockbackVector.y = knockbackVelocity.y = 0;

	// ノックバック用のフラグ初期化
	knockbackFlag.Initialize();
	knockbackFlag.SetMaxTimeProperty(KNOCKBACK_TIME);
	knockbackFlag.SetFlag(true);

	// 無敵時間用のフラグ初期化
	invincibleFlag.Initialize();
	invincibleFlag.SetMaxTimeProperty(INVINCIBLE_TIME);
	invincibleFlag.SetFlag(true);

	HitStopManager::GetInstance()->Active(0.5f);
}

void PlayerComponent::UpdateKnockback(float deltaTime)
{
	if (knockbackFlag.GetFlag() == false)return;

	if (knockbackFlag.GetOverTimeTrigger())
	{
		knockbackFlag.SetFlag(false);
		knockbackFlag.SetTime(knockbackFlag.GetMaxTimeProperty());
	}

	// ノックバックの経過時間を取得し線形補間でノックバックのベクトルを減衰させる
	float lerpTime = knockbackFlag.GetTime() / knockbackFlag.GetMaxTimeProperty();
	knockbackVelocity = GE::Math::Vector3::Lerp(setKnockbackVector, GE::Math::Vector3(), lerpTime);

	knockbackFlag.Update(deltaTime);
}

bool PlayerComponent::CheckMovable()
{
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//左のみ、画面の中央まで移動可能
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D) == false &&
			transform->position.x > Tutorial::FIRST_PLAYER_POS_X;
	}
	else if(Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//移動させない
		return false;
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//1秒後、上側の特定の位置以外で移動可能 (そこに誘導させる)
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A) == false &&
			Tutorial::GetTutorialTimer() >= 1 &&
			!(moveEntity.GetStanceState() == StanceState::INVERSE &&
			transform->position.x >= Tutorial::THIRD_PLAYER_POS);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//1秒後、上側の特定の位置以外で移動可能 (そこに誘導させる)
		return Tutorial::GetTutorialTimer() >= 1 &&
			!(moveEntity.GetStanceState() == StanceState::INVERSE &&
			transform->position.x >= Tutorial::FOURTH_PLAYER_POS_X1 && transform->position.x < Tutorial::FOURTH_PLAYER_POS_X2);
	}

	//チュートリアル外では自由に移動できる
	return true;
}

void PlayerComponent::UpdateAttackable()
{
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//画面の中央より左にいるとき攻撃可能
		Tutorial::SetAttackable(transform->position.x <= Tutorial::FIRST_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//Shadow側で管理するのでなにもしない
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//上側の特定の位置で攻撃可能
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::INVERSE && 
			transform->position.x >= Tutorial::THIRD_PLAYER_POS);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//上側の特定の位置で攻撃可能
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::INVERSE &&
			transform->position.x >= Tutorial::FOURTH_PLAYER_POS_X1 && transform->position.x < Tutorial::FOURTH_PLAYER_POS_X2);
	}
}

//void PlayerComponent::OnGui()
//{
//	float dragSpeed = 0.1f;
//	float maxValue = 100;
//	ImGui::DragFloat3("Angles", transform->rotation.value, dragSpeed, 0, 360);
//}
