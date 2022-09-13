#include "BossEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GameFramework/Component/BoxCollider.h>
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/GUI\GUIManager.h        >
#include "NormalEnemyComponent.h"
#include "CollisionManager.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"
#include "EffectManager.h"
#include "Camera2D.h"
#include "Tutorial.h"
#include "GameUtility.h"
#include "Result.h"

const GE::Math::Vector3 BossEnemyComponent::SPRITE_SIZE = { 512, 384, 0 };
const float BossEnemyComponent::MIN_SCALE = 0.5f;
const float BossEnemyComponent::MOVE_SPEED = 0.5f;

const int BossEnemyComponent::MAX_ANIMATION_NUMBER = 3;
const int BossEnemyComponent::DAMAGE_ANIMATION_NUMBER = 3;

const float BossEnemyComponent::CHANGE_ANIMATION_TIME = 0.25f;

const GE::Math::Vector2 BossEnemyComponent::TEXTURE_SIZE = {2048,384};
const GE::Math::Vector2 BossEnemyComponent::CLIP_SIZE = {512,384};

int BossEnemyComponent::generateCountOneAttack = 0;

void BossEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2, 1080 / 2, 0 };

	Initialize();

	damageFlag.Initialize();
	damageFlag.SetMaxTimeProperty(1);
}

void BossEnemyComponent::Update(float deltaTime)
{
	beforeIsHitPlayer = isHitPlayer;
	if (HitStopManager::GetInstance()->IsActive() == false)
	{
		// 前回のフレームのプレイヤーとのヒット状況を保持
		isHitPlayer = false;
	}

	//タイマー更新
	UpdateTimer(deltaTime);

	//スケール更新
	UpdateScale();

	//ライフ更新
	UpdateLife();

	//移動処理
	Move();

#ifdef _DEBUG
	//デバッグ用　ほんとは攻撃されたら呼び出す
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F1)) {
		isGenerate = true;
	}
#endif // DEBUG

	UpdateAnimation(deltaTime);
	UpdateDamageFlag(deltaTime);
}

void BossEnemyComponent::LateDraw()
{
	//死んでいるなら描画しない
	if (IsDead()) { return; }

	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteTextureAnimationShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &Camera2D::GetInstance()->GetCameraInfo(), sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("boss_enemy")->GetSRVNumber() });

	GE::TextureAnimationInfo animationInfo;
	animationInfo.clipSize = CLIP_SIZE;
	animationInfo.pivot = { (float)drawAnimationNumber,0 };
	animationInfo.textureSize = TEXTURE_SIZE;
	renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

	graphicsDevice->DrawMesh("2DPlane");
}

void BossEnemyComponent::OnCollision(GE::GameObject* other)
{
	if (other->GetTag() != "Player")return;
	if (PlayerAttackManager::GetInstance()->GetAttackState() != PlayerAttackState::ACTIVE)return;

	// 前回のフレームでヒットしているならヒット判定を取らない
	if (beforeIsHitPlayer == true)
	{
		isHitPlayer = true;
		return;
	}

	isGenerate = true;
	generateNum = 1;
	isHitPlayer = true;
	HitStopManager::GetInstance()->Active(0.5f);


	Camera2D::GetInstance()->Shake(0.4f, 50);
	HitStopManager::GetInstance()->Active(0.75f);
	EffectManager::GetInstance()->Active("slashEffect",transform->position);
	EffectManager::GetInstance()->Active("dotEffect", transform->position);
	//チュートリアル状態遷移用
	if (Tutorial::IsEndTutorial() == false){
		if (Tutorial::GetTutorialState() == TutorialState::FIRST_ATTACK) {
			Tutorial::DecrementChangeStateCount(1);
		}
		else if (Tutorial::GetTutorialState() == TutorialState::SECOND_ATTACK) {
			Tutorial::DecrementChangeStateCount(2);
		}
	}
}

void BossEnemyComponent::OnGui()
{
	ImGui::Text("Life:%d", life);
}

void BossEnemyComponent::Initialize()
{
	//最大生成数初期化
	maxGenerateCount = 3;

	//チュートリアルスキップ時は最大生成数からチュートリアル分を引く
	if (Tutorial::IsSkipTutorial()) {
		maxGenerateCount -= 2;
	}

	//初期ライフ
	life = maxGenerateCount;

	//敵生成カウントリセット
	alreadyGeneratedCount = 0;

	//一度に生成する敵の数リセット
	generateNum = 1;

	//初期状態は動かない
	velocity = 0;

	//初期は通常の大きさ
	scaleDownMag = 1.0f;

	//タイマーは最大値でリセット
	scaleDownTimer = 1;
	
	isGenerate = false;
	
	normalEnemies.clear();
}

void BossEnemyComponent::Move()
{
	//ライフが半分以下になったら動き出す
	const int START_MOVE_LIFE = Tutorial::IsSkipTutorial() ? maxGenerateCount / 2 : (maxGenerateCount - 2) / 2;
	if (life <= START_MOVE_LIFE && velocity == 0) {
		velocity = (float)(GE::RandomMaker::GetInt(0, 1) * 2 - 1);
	}

	//横移動
	transform->position.x += velocity * MOVE_SPEED;

	//方向転換
	if (transform->position.x < transform->scale.x / 2) {
		transform->position.x = transform->scale.x / 2;
		velocity *= -1;
	}
	else if (transform->position.x > 1920 - transform->scale.x / 2) {
		transform->position.x = 1920 - transform->scale.x / 2;
		velocity *= -1;
	}
}

void BossEnemyComponent::UpdateTimer(float deltaTime)
{
	scaleDownTimer = scaleDownTimer + deltaTime < 1.0f ? scaleDownTimer + deltaTime : 1.0f;
}

void BossEnemyComponent::UpdateScale()
{
	//イージングかけてスケール倍率変化
	float t = scaleDownTimer;
	float beforeScale = scaleDownMag + ((1.0 - MIN_SCALE) / maxGenerateCount) * GameUtility::GetNowChain();
	float scaleMag = beforeScale + (scaleDownMag - beforeScale) * GE::Math::Easing::EaseOutQuart(t);

	//自身のスケールを小さくする
	transform->scale = SPRITE_SIZE * scaleMag;
}

void BossEnemyComponent::UpdateLife()
{
	static int prevLife = life;
	//エネミーリスト走査して死んでいたら削除、ライフ減少
	for (int i = 0; i < normalEnemies.size();) {
		if (normalEnemies[i]->IsDead()) {
			life--;
			normalEnemies.erase(normalEnemies.begin() + i);
		}
		else {
			i++;
		}
	}

	//ライフ変動で敵生成
	if (life != prevLife) {
		isGenerate = true;

		generateNum = (prevLife - life);
		generateCountOneAttack += (prevLife - life);

		//チェイン数によって生成数ボーナスを付ける
		if (generateCountOneAttack >= 3) {
			generateNum += (prevLife - life);
		}
	}

	prevLife = life;
	
	//ライフ0以下でリザルトへ
	if (life <= 0 && GameUtility::GetGameState() == GameState::GAME) {
		GameUtility::TimerStop();
		Result::SendScore(GameUtility::GetClearTime());
		Result::GetRanking();
		GameUtility::SetGameState(GameState::RESULT);
	}
}

void BossEnemyComponent::UpdateDamageFlag(float deltaTime)
{
	if (damageFlag.GetOverTimeTrigger())
	{
		damageFlag.SetFlag(false);
		damageFlag.SetTime(0);
	}
	if (damageFlag.GetFlag() == true)
	{
		drawAnimationNumber = DAMAGE_ANIMATION_NUMBER;
	}

	damageFlag.Update(deltaTime);
}
void BossEnemyComponent::UpdateAnimation(float deltaTime)

{
	// アニメーション用のタイマー更新
	if (drawAnimationTimer >= CHANGE_ANIMATION_TIME)
	{
		drawAnimationTimer = 0;
		drawAnimationNumber++;
		if (drawAnimationNumber >= DAMAGE_ANIMATION_NUMBER)
		{
			drawAnimationNumber = 0;
		}
	}
	drawAnimationTimer += deltaTime;
}

void BossEnemyComponent::GenerateNormalEnemy()
{
	//前フレームで生成フラグ立ってなかったらreturn
	if (isGenerate == false) { return; }

	//すでに最大生成回数以上であったらreturn
	if (alreadyGeneratedCount >= maxGenerateCount) { return; }

	for (int i = 0; i < generateNum; i++) {
		//敵生成位置設定
		GE::Math::Vector3 afterPos = {};

		//チュートリアル中は固定座標に出現させる
		if ((int)Tutorial::GetTutorialState() == (int)TutorialState::FIRST_ATTACK + 1) {
			afterPos = Tutorial::POS_GENERATE_ENEMY_1;
		}
		else if ((int)Tutorial::GetTutorialState() == (int)TutorialState::SECOND_ATTACK + 1) {
			afterPos = Tutorial::POS_GENERATE_ENEMY_2;
		}
		else {
			//ボスにだいたい重ならない範囲でランダムに移動座標決定
			float x = GE::RandomMaker::GetFloat(1920 / 4, 1920 / 4 + 1920 / 2);

			float y = GE::RandomMaker::GetFloat(100, 300);
			if (GE::RandomMaker::GetInt(0, 1) == 1) {
				y *= -1;
			}
			afterPos = { x, 1080 / 2 + y, 0 };
		}


		//NormalEnemy生成
		auto* newEnemy = pGameObjectManager->AddGameObject(new GE::GameObject());
		newEnemy->SetName("NormalEnemy_" + std::to_string(alreadyGeneratedCount));
		newEnemy->GetTransform()->position = { 0,0,0 };
		newEnemy->SetDrawAxisEnabled(true);
		auto* sampleCollider = newEnemy->AddComponent<GE::BoxCollider>();
		auto* normalEnemyComponent = newEnemy->AddComponent<NormalEnemyComponent>();
		normalEnemyComponent->SetPAudioManager(pAudioManager);
		normalEnemyComponent->SetPBossPosition(&transform->position);
		normalEnemyComponent->SetPPlayerMoveEntity(pPlayerMoveEntity);
		normalEnemyComponent->SetPPlayerPos(pPlayerPos);
		normalEnemyComponent->SetMovePos(transform->position, afterPos);
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		sampleCollider->SetType(GE::ColliderType::OBB);

		newEnemy->SetTag("Enemy");
		CollisionManager::GetInstance()->AddEnemy(newEnemy, sampleCollider);

		newEnemy->Awake();
		newEnemy->Start();

		normalEnemies.push_back(normalEnemyComponent);

		//スケール値下げる
		scaleDownMag -= (1.0 - MIN_SCALE) / maxGenerateCount;

		//生成数増やす
		alreadyGeneratedCount++;

		//最大生成数に達したらbreak
		if (alreadyGeneratedCount >= maxGenerateCount) { break; }
	}

	//タイマーリセット
	scaleDownTimer = 0;

	//敵生成タイミングで効果音再生する
	pAudioManager->Use("Explosion")->Start();

	isGenerate = false;

	damageFlag.SetFlag(true);
	damageFlag.SetTime(0);
}
