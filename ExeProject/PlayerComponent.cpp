#include "PlayerComponent.h"
#include "GameSetting.h"
#include "HitStopManager.h"
#include "Camera2D.h"
#include "Tutorial.h"
#include "EffectManager.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

const int PlayerComponent::MAX_ANIMATION_NUMBER_WALK = 8;
const int PlayerComponent::MAX_ANIMATION_NUMBER_STOP = 4;
const float PlayerComponent::CHANGE_ANIMATION_TIME_WALK = 0.25f / 2;
const float PlayerComponent::CHANGE_ANIMATION_TIME_STOP = 0.25f;

const GE::Math::Vector2 PlayerComponent::TEXTURE_SIZE_WALK = { 768,96 };
const GE::Math::Vector2 PlayerComponent::TEXTURE_SIZE_STOP = { 384,96 };
const GE::Math::Vector2 PlayerComponent::CLIP_SIZE = { 96 };

const float PlayerComponent::FLASHING_TIME = 0.2f;

const int PlayerComponent::MAX_HP = 3;
const float PlayerComponent::MOVE_SPEED = 5;

PlayerComponent::PlayerComponent()
	: inputDevice(nullptr)
	, moveEntity(MoveEntity())
	, hp(3)
{
}

void PlayerComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	moveEntity.Initialize();

	const float SPRITE_SIZE = 100;
	transform->scale = SPRITE_SIZE;

		transform->position = { 1920 * 5 / 8, GE::Window::GetWindowSize().y - transform->scale.y / 2,0 };

	hp = MAX_HP;
}

void PlayerComponent::Update(float deltaTime)
{
	const float GAME_TIME = GameSetting::GetInstance()->GetTime();
	bool isBeforeIsMove = isMove;

	//チュートリアルなどで移動不可状態かチェック
	if (CheckMovable()) {
		Move(GAME_TIME);

		// アニメーションリセット
		if (isMove != isBeforeIsMove)
		{
			drawAnimationNumber = 0;
		}

		const float MAX_ANIMATION_TIME = isMove ? CHANGE_ANIMATION_TIME_WALK : CHANGE_ANIMATION_TIME_STOP;
		if (drawAnimationTimer >= MAX_ANIMATION_TIME)
		{
			drawAnimationTimer = 0;
			++drawAnimationNumber;

			const int MAX_ANIMATION_NUMBER = isMove ? MAX_ANIMATION_NUMBER_WALK : MAX_ANIMATION_NUMBER_STOP;
			if (drawAnimationNumber >= MAX_ANIMATION_NUMBER)
			{
				drawAnimationNumber = 0;
			}
		}

		drawAnimationTimer += deltaTime;
	}

	transform->position += knockbackVelocity * GAME_TIME;

	// 移動オブジェクト用の各種更新処理
	moveEntity.CheckTeleport(transform->position, transform->scale);
	moveEntity.UpdateChangeDirectionFlag(deltaTime, GAME_TIME);
	moveEntity.UpdateStanceAngle(deltaTime, GAME_TIME);

	UpdateInvinsible(deltaTime * GAME_TIME);

	UpdateKnockback(deltaTime * GAME_TIME);

	//チュートリアル時攻撃可能かチェック
	UpdateAttackable();
}

void PlayerComponent::LateDraw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteTextureAnimationShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(transform->rotation);
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);
	transform->rotation = moveEntity.GetAngles();

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();
	if (isFlashing)
	{
		material.color = GE::Color(0.5f, 1);
	}

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &Camera2D::GetInstance()->GetCameraInfo(), sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });


	int resourceNumber = isMove ? graphicsDevice->GetTextureManager()->Get("texture_player_walk")->GetSRVNumber() : graphicsDevice->GetTextureManager()->Get("texture_player")->GetSRVNumber();
	renderQueue->AddSetShaderResource({ 4,resourceNumber });

	GE::TextureAnimationInfo animationInfo;
	animationInfo.clipSize = CLIP_SIZE;
	animationInfo.pivot = { (float)drawAnimationNumber,0 };
	animationInfo.textureSize = isMove ? TEXTURE_SIZE_WALK : TEXTURE_SIZE_STOP;
	renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

	graphicsDevice->DrawMesh("2DPlane");

	DrawHP();
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

	--hp;
	if (hp <= 0)isDead = true;

	const float POWER = 10;
	const float KNOCKBACK_TIME = 0.5f;
	const float INVINCIBLE_TIME = 2.0f;

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
	EffectManager::GetInstance()->Active("dotEffect", transform->position + -knockbackVelocity);
	audioManager->Use("PlayerHit")->Start();
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
		//左のみ、指定位置まで移動可能
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D) == false &&
			!(transform->position.x <= Tutorial::FIRST_PLAYER_POS_X);
	}
	else if(Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//右のみ、指定位置まで移動可能
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A) == false &&
			!(transform->position.x >= Tutorial::SECOND_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//右のみ、下側の指定位置まで移動可能
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A) == false &&
			!(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x >= Tutorial::THIRD_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//右のみ、下側の特定の位置以外で移動可能
		return inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A) == false &&
			!(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x >= Tutorial::FOURTH_PLAYER_POS_X - 5 && transform->position.x < Tutorial::FOURTH_PLAYER_POS_X + 5);
	}

	//チュートリアル外では自由に移動できる
	return true;
}

void PlayerComponent::UpdateAttackable()
{
	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//指定位置より左にいるとき攻撃可能
		Tutorial::SetAttackable(transform->position.x <= Tutorial::FIRST_PLAYER_POS_X, 0);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//指定位置より右にいるとき攻撃可能
		Tutorial::SetAttackable(transform->position.x >= Tutorial::SECOND_PLAYER_POS_X, 0);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//下側の特定の位置で攻撃可能
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::NORMAL && 
			transform->position.x >= Tutorial::THIRD_PLAYER_POS_X, 0);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//下側の特定の位置で攻撃可能
		Tutorial::SetAttackable(moveEntity.GetStanceState() == StanceState::NORMAL &&
			transform->position.x >= Tutorial::FOURTH_PLAYER_POS_X - 5 && transform->position.x < Tutorial::FOURTH_PLAYER_POS_X + 5, 0);
	}
}

void PlayerComponent::SetAudioManager(GE::AudioManager* pAudioManager)
{
	audioManager = pAudioManager;
}

void PlayerComponent::UpdateInvinsible(float deltaTime)
{
	if (invincibleFlag.GetFlag() == false)return;

	// 点滅切り替え
	if (flashingTimer > FLASHING_TIME)
	{
		isFlashing = !isFlashing;
		flashingTimer = 0;
	}

	// 無敵時間終了
	if (invincibleFlag.GetOverTimeTrigger())
	{
		invincibleFlag.Initialize();
		isFlashing = false;
		flashingTimer = 0;
	}

	flashingTimer += deltaTime;
	invincibleFlag.Update(deltaTime);
}

void PlayerComponent::Move(const float GAME_TIME)
{
	isMove = false;
	auto keyboard = inputDevice->GetKeyboard();
	auto ctrler = inputDevice->GetXCtrler();

	// 移動方向の変更テスト
	if(moveEntity.GetDirectionState() == MoveDirectionState::RIGHT && (keyboard->CheckHitKey(GE::Keys::A) || keyboard->CheckHitKey(GE::Keys::LEFT) || ctrler->GetLStickX() < 0))
	{
		moveEntity.ChangeMoveDirection();
	}
	else if (moveEntity.GetDirectionState() == MoveDirectionState::LEFT && (keyboard->CheckHitKey(GE::Keys::D) || keyboard->CheckHitKey(GE::Keys::RIGHT) || ctrler->GetLStickX() > 0))
	{
		moveEntity.ChangeMoveDirection();
	}


	GE::Math::Vector3 moveVector = {0,0,0};
	if (keyboard->CheckHitKey(GE::Keys::A) || keyboard->CheckHitKey(GE::Keys::LEFT) || ctrler->GetLStickX() < 0)
	{
		isMove = true;
		moveVector = { -1,0,0 };
		moveVector *= MOVE_SPEED * GAME_TIME;
	}
	if (keyboard->CheckHitKey(GE::Keys::D) || keyboard->CheckHitKey(GE::Keys::RIGHT) || ctrler->GetLStickX() > 0)
	{
		isMove = true;
		moveVector = { 1,0,0 };
		moveVector *= MOVE_SPEED * GAME_TIME;
	}

	transform->position += moveVector;
}

void PlayerComponent::DrawHP()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteTextureAnimationShader");
	
	GE::Math::Matrix4x4 modelMatrix;

	GE::TextureAnimationInfo animationInfo;
	animationInfo.textureSize = { 32,16 };
	animationInfo.clipSize = 16;

	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("texture_player_heart")->GetSRVNumber() });

	const float DRAW_SIZE = 32;
	const float ANGLE = moveEntity.GetStanceState() == StanceState::NORMAL ? 0 : 180;
	const float HEIGHT = moveEntity.GetStanceState() == StanceState::NORMAL ? 1 : -1;

	for (int i = 0; i < 3; ++i)
	{
		modelMatrix = GE::Math::Matrix4x4::Scale(DRAW_SIZE);
		modelMatrix *= GE::Math::Matrix4x4::RotationX(GE::Math::ConvertToRadian(ANGLE));

		GE::Math::Vector3 drawPosition;
		const float SPACE = 32;

		// 3つのHPをプレイヤーの上で分散
		drawPosition = transform->position - GE::Math::Vector3(1, 0, 0) * SPACE * (i - 1);
		drawPosition -= GE::Math::Vector3(0, HEIGHT, 0) * (SPACE + DRAW_SIZE);
		modelMatrix *= GE::Math::Matrix4x4::Translate(drawPosition);

		// 描画するアニメーション番号の指定
		int drawAnimationNum = (i >= hp) ? 1 : 0;
		animationInfo.pivot.x = drawAnimationNum;

		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });
		graphicsDevice->DrawMesh("2DPlane");
	}
}

void PlayerComponent::OnGui()
{
	float dragSpeed = 0.1f;
	float maxValue = 100;
	ImGui::Text("%d", drawAnimationNumber);
}
