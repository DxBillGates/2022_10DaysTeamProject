#include "BossEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GameFramework/Component/BoxCollider.h>
#include <GatesEngine/Header/Util/Random.h           >
#include "NormalEnemyComponent.h"
#include "CollisionManager.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"
#include "Tutorial.h"

const GE::Math::Vector3 BossEnemyComponent::SPRITE_SIZE = { 512, 384, 0 };
const float BossEnemyComponent::MIN_SCALE = 0.5f;
const float BossEnemyComponent::MOVE_SPEED = 0.5f;

void BossEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2, 1080 / 2, 0 };

	//初期ライフは最大敵生成数
	life = MAX_GENERATE_COUNT;

	//初期状態は動かない
	velocity = 0;

	//初期は通常の大きさ
	scaleDownMag = 1.0f;
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

	//デバッグ用　ほんとは攻撃されたら呼び出す
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F1)) {
		isGenerate = true;
	}
}

void BossEnemyComponent::LateDraw()
{
	//死んでいるなら描画しない
	if (IsDead()) { return; }

	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("boss_enemy")->GetSRVNumber() });

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
	isHitPlayer = true;
	HitStopManager::GetInstance()->Active(0.5f);

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

void BossEnemyComponent::Move()
{
	//ライフが一定値より低下したら動き出す
	if (life <= MAX_GENERATE_COUNT / 2 && velocity == 0) {
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
	float beforeScale = scaleDownMag + (1.0 - MIN_SCALE) / MAX_GENERATE_COUNT;
	float scaleMag = beforeScale + (scaleDownMag - beforeScale) * GE::Math::Easing::EaseOutQuart(t);

	//自身のスケールを小さくする
	transform->scale = SPRITE_SIZE * scaleMag;
}

void BossEnemyComponent::UpdateLife()
{
	//エネミーリスト走査して死んでいる個体の数だけライフ値減少させる
	//(コスト高めなので処理見直したほうがいいかも)
	life = MAX_GENERATE_COUNT;
	for (auto& v : normalEnemies) {
		if (v->IsDead()) {
			life--;
		}
	}
}

void BossEnemyComponent::GenerateNormalEnemy()
{
	//前フレームで生成フラグ立ってなかったらreturn
	if (isGenerate == false) { return; }

	//すでに最大生成回数以上であったらreturn
	if (normalEnemies.size() >= MAX_GENERATE_COUNT) { return; }

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
	newEnemy->SetName("normal_enemy_" + std::to_string(normalEnemies.size()));
	newEnemy->GetTransform()->position = { 0,0,0 };
	newEnemy->SetDrawAxisEnabled(true);
	auto* sampleCollider = newEnemy->AddComponent<GE::BoxCollider>();
	auto* normalEnemyComponent = newEnemy->AddComponent<NormalEnemyComponent>();
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
	scaleDownMag -= (1.0 - MIN_SCALE) / MAX_GENERATE_COUNT;

	//タイマーリセット
	scaleDownTimer = 0;

	isGenerate = false;
}
