#include "PlayerComponent.h"
#include "GameSetting.h"
#include "HitStopManager.h"
#include "Camera2D.h"
#include "Tutorial.h"
#include "EffectManager.h"
#include "PlayerAttackManager.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

const int PlayerComponent::MAX_ANIMATION_NUMBER_WALK = 8;
const int PlayerComponent::MAX_ANIMATION_NUMBER_STOP = 4;
const int PlayerComponent::MAX_ANIMATION_NUMBER_DEAD = 2;

const float PlayerComponent::CHANGE_ANIMATION_TIME_WALK = 0.25f / 2;
const float PlayerComponent::CHANGE_ANIMATION_TIME_STOP = 0.25f;
const float PlayerComponent::CHANGE_ANIMATION_TIME_DEAD = 1.5f;

const GE::Math::Vector2 PlayerComponent::TEXTURE_SIZE_WALK = { 768,96 };
const GE::Math::Vector2 PlayerComponent::TEXTURE_SIZE_STOP = { 384,96 };
const GE::Math::Vector2 PlayerComponent::TEXTURE_SIZE_DEAD = { 192,96 };
const GE::Math::Vector2 PlayerComponent::CLIP_SIZE = { 96 };

const float PlayerComponent::FLASHING_TIME = 0.2f;

const int PlayerComponent::MAX_HP = 3;
const float PlayerComponent::MOVE_SPEED = 10;

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

	const float SPRITE_SIZE = 96;
	transform->scale = SPRITE_SIZE;

	transform->position = { 1920 * 5 / 8, GE::Window::GetWindowSize().y - transform->scale.y / 2,0 };

	hp = MAX_HP;

	isMove = false;
	isDead = false;

	flashingTimer = 0;
	isFlashing = false;

	invincibleFlag.Initialize();

	isTrueDeadFlagContrller.SetFlag(false);
	isTrueDeadFlagContrller.SetTime(0);
	isTrueDeadFlagContrller.SetMaxTimeProperty(CHANGE_ANIMATION_TIME_DEAD);

	isTrueDead = false;
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

		float MAX_ANIMATION_TIME = isMove ? CHANGE_ANIMATION_TIME_WALK : CHANGE_ANIMATION_TIME_STOP;
		MAX_ANIMATION_TIME = isDead || isTrueDead ? CHANGE_ANIMATION_TIME_DEAD : MAX_ANIMATION_TIME;
		if (drawAnimationTimer >= MAX_ANIMATION_TIME)
		{
			drawAnimationTimer = 0;
			++drawAnimationNumber;

			int MAX_ANIMATION_NUMBER = isMove ? MAX_ANIMATION_NUMBER_WALK : MAX_ANIMATION_NUMBER_STOP;
			MAX_ANIMATION_NUMBER = isDead || isTrueDead ? MAX_ANIMATION_NUMBER_DEAD : MAX_ANIMATION_NUMBER;
			if (drawAnimationNumber >= MAX_ANIMATION_NUMBER)
			{
				drawAnimationNumber = 0;
				if (isDead || isTrueDead)
				{
					drawAnimationNumber = MAX_ANIMATION_NUMBER_DEAD - 1;
				}
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

	UpdateTrueDeadFlag(deltaTime);
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
	resourceNumber = isDead || isTrueDead ? graphicsDevice->GetTextureManager()->Get("texture_player_dead")->GetSRVNumber() : resourceNumber;

	renderQueue->AddSetShaderResource({ 4,resourceNumber });

	GE::TextureAnimationInfo animationInfo;
	animationInfo.clipSize = CLIP_SIZE;
	animationInfo.pivot = { (float)drawAnimationNumber,0 };
	animationInfo.textureSize = isMove ? TEXTURE_SIZE_WALK : TEXTURE_SIZE_STOP;
	animationInfo.textureSize = isDead || isTrueDead ? TEXTURE_SIZE_DEAD : animationInfo.textureSize;
	renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

	graphicsDevice->DrawMesh("2DPlane");

	DrawHP();
}

void PlayerComponent::OnCollision(GE::GameObject* other)
{
	if (isDead == true)return;

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

	float POWER = 10;
	const float KNOCKBACK_TIME = 0.5f;
	const float INVINCIBLE_TIME = 2.0f;

	HitStopManager::GetInstance()->Active(0.5f);
	EffectManager::GetInstance()->Active("dotEffect", transform->position + -knockbackVelocity);
	audioManager->Use("PlayerHit")->Start();

	if (hp <= 0)
	{
		isDead = true;
		HitStopManager::GetInstance()->Active(1.f);
		POWER = 20;
		isTrueDeadFlagContrller.SetFlag(true);
		isTrueDeadFlagContrller.SetTime(0);
		drawAnimationNumber = 0;
	}

	setKnockbackVector = knockbackVelocity = GE::Math::Vector3::Normalize(transform->position - otherPosition) * POWER;
	setKnockbackVector.y = knockbackVelocity.y = 0;

	// ノックバック用のフラグ初期化
	knockbackFlag.Initialize();
	knockbackFlag.SetMaxTimeProperty(KNOCKBACK_TIME);
	knockbackFlag.SetFlag(true);

	// 無敵時間用のフラグ初期化
	if (isDead == false)
	{
		invincibleFlag.Initialize();
		invincibleFlag.SetMaxTimeProperty(INVINCIBLE_TIME);
		invincibleFlag.SetFlag(true);
	}
}

void PlayerComponent::UpdateKnockback(float deltaTime)
{
	if (knockbackFlag.GetFlag() == false)return;

	if (knockbackFlag.GetOverTimeTrigger())
	{
		knockbackFlag.SetFlag(false);
		knockbackFlag.SetTime(1);
	}

	// ノックバックの経過時間を取得し線形補間でノックバックのベクトルを減衰させる
	float lerpTime = knockbackFlag.GetTime()/* / knockbackFlag.GetMaxTimeProperty()*/;
	knockbackVelocity = GE::Math::Vector3::Lerp(setKnockbackVector, GE::Math::Vector3(), lerpTime);

	knockbackFlag.Update(deltaTime);
}

bool PlayerComponent::CheckMovable()
{
	bool isLeftMove = inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A);
	if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::LEFT))isLeftMove = true;
	if (inputDevice->GetXCtrler()->GetLStickX() < -0.15f)isLeftMove = true;

	bool isRightMove = inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D);
	if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::RIGHT))isRightMove = true;
	if (inputDevice->GetXCtrler()->GetLStickX() > 0.15f)isRightMove = true;

	if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
		//左のみ、指定位置まで移動可能
		return isRightMove == false &&
			!(transform->position.x <= Tutorial::FIRST_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
		//右のみ、指定位置まで移動可能
		return isLeftMove == false &&
			!(transform->position.x >= Tutorial::SECOND_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::THIRD_ATTACK) {
		//右のみ、下側の指定位置まで移動可能
		return isLeftMove == false &&
			!(moveEntity.GetStanceState() == StanceState::NORMAL &&
				transform->position.x >= Tutorial::THIRD_PLAYER_POS_X);
	}
	else if (Tutorial::GetTutorialState() == TutorialState::FOURTH_ATTACK) {
		//右のみ、下側の特定の位置以外で移動可能
		return isLeftMove == false &&
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

void PlayerComponent::SetPAudioManager(GE::AudioManager* pAudioManager)
{
	audioManager = pAudioManager;
}

bool PlayerComponent::IsDead()
{
	return isDead | isTrueDead;
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

void PlayerComponent::UpdateTrueDeadFlag(float deltaTime)
{
	if (isTrueDead == true)
	{
		drawAnimationNumber = MAX_ANIMATION_NUMBER_DEAD - 1;
	}

	if (isTrueDeadFlagContrller.GetFlag() == false)return;

	if (isTrueDeadFlagContrller.GetOverTimeTrigger())
	{
		isTrueDead = true;
		GameUtility::SetGameState(GameState::RESULT_GAMEOVER);
	}
	isTrueDeadFlagContrller.Update(deltaTime);
}

void PlayerComponent::Move(const float GAME_TIME)
{
	if (isDead || isTrueDead)return;

	isMove = false;
	auto keyboard = inputDevice->GetKeyboard();
	auto ctrler = inputDevice->GetXCtrler();

	// 移動方向の変更テスト
	if (PlayerAttackManager::GetInstance()->GetAttackState() == PlayerAttackState::NONE)
	{
		if (moveEntity.GetDirectionState() == MoveDirectionState::RIGHT && (keyboard->CheckHitKey(GE::Keys::A) || keyboard->CheckHitKey(GE::Keys::LEFT) || ctrler->GetLStickX() < 0))
		{
			moveEntity.ChangeMoveDirection();
		}
		else if (moveEntity.GetDirectionState() == MoveDirectionState::LEFT && (keyboard->CheckHitKey(GE::Keys::D) || keyboard->CheckHitKey(GE::Keys::RIGHT) || ctrler->GetLStickX() > 0))
		{
			moveEntity.ChangeMoveDirection();
		}
	}


	GE::Math::Vector3 moveVector = { 0,0,0 };
	if (keyboard->CheckHitKey(GE::Keys::A) || keyboard->CheckHitKey(GE::Keys::LEFT) || ctrler->GetLStickX() < -0.15f)
	{
		isMove = true;
		moveVector = { -1,0,0 };
		moveVector *= MOVE_SPEED * GAME_TIME;
	}
	if (keyboard->CheckHitKey(GE::Keys::D) || keyboard->CheckHitKey(GE::Keys::RIGHT) || ctrler->GetLStickX() > 0.15f)
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

	for (int i = 0; i < MAX_HP; ++i)
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
	ImGui::Text("isTrueTimer %f", isTrueDeadFlagContrller.GetTime());
}
