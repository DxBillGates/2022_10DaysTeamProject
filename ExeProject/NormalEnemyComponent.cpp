#include "NormalEnemyComponent.h"
#include "PlayerAttackManager.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GUI\GUIManager.h>
#include <GatesEngine/Header/Util/Random.h>

const float NormalEnemyComponent::INIT_SCALE = 100;
const float NormalEnemyComponent::WALK_SPEED = INIT_SCALE;
const float NormalEnemyComponent::MAX_FLYING_LOOP_TIMER = 2.0f;
const float NormalEnemyComponent::MAX_WALK_LOOP_TIMER = 1.5f;
const float NormalEnemyComponent::MAX_FLYING_ANIME_LOOP_TIMER = 0.3f;
const float NormalEnemyComponent::FLYING_ANIME_LOOP_TIMER_SPEED = 0.75f;
const float NormalEnemyComponent::MAX_WALKING_ANIME_LOOP_TIMER = 0.3f;
const float NormalEnemyComponent::WALKING_ANIME_LOOP_TIMER_SPEED = 0.75f;

void NormalEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	const float SPRITE_SIZE = INIT_SCALE;

	transform->scale = SPRITE_SIZE;
	transform->position = moveBeforePos;

	moveTimer = 0;
	flyingLoopTimer = 0;
	walkingLoopTimer = 0;
	flyingAnimeLoopTimer = 0;
	walkingAnimeLoopTimer = 0;
}

void NormalEnemyComponent::Update(float deltaTime)
{
	//タイマー更新
	UpdateTimer(deltaTime);

	//画像の向きを正す
	CheckStance();

	//状態に応じて処理を分ける
	if (enemyState == EnemyState::GENERATING) {
		UpdateGenerating();
	}
	else if (enemyState == EnemyState::FLYING) {
		UpdateFlying();
	}
	else if (enemyState == EnemyState::FALLING) {
		UpdateFalling();
	}
	else if (enemyState == EnemyState::WALKING) {
		UpdateWalking();
	}
	else {
		UpdateDeading();
	}
}

void NormalEnemyComponent::LateDraw()
{
	//死んだら描画しない
	if (enemyState == EnemyState::DEAD) { return; }

	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	if (stanceState == StanceState::NORMAL) {
		modelMatrix *= GE::Math::Matrix4x4::RotationZXY({ 0, 0, 0 });
	}
	else {
		modelMatrix *= GE::Math::Matrix4x4::RotationZXY({ 180, 0, 0 });
	}

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });

	//状態によって送る画像を変える
	if (enemyState == EnemyState::GENERATING ||
		enemyState == EnemyState::FLYING) {
		int drawNum = flyingAnimeLoopTimer * 10 <= 2 ? flyingAnimeLoopTimer * 10 : 2;
		renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy_flying_" + std::to_string(drawNum))->GetSRVNumber() });
	}
	else if (enemyState == EnemyState::FALLING) {
		renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy_flying_damage")->GetSRVNumber() });
	}
	else if (enemyState == EnemyState::WALKING) {
		int drawNum = walkingAnimeLoopTimer * 10 <= 2 ? walkingAnimeLoopTimer * 10 : 2;
		renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy_walking_" + std::to_string(drawNum))->GetSRVNumber() });
	}
	else {
		renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy_walking_damage")->GetSRVNumber() });
	}

	graphicsDevice->DrawMesh("2DPlane");
}

void NormalEnemyComponent::OnCollision(GE::GameObject* other)
{
	if (other->GetTag() == "Player" || other->GetTag() == "ShadowPlayer")
	{
		if (PlayerAttackManager::GetInstance()->GetAttackState() != PlayerAttackState::ACTIVE)return;
		if (enemyState == EnemyState::FLYING)
		{
			//デバッグ用　状態遷移
			//上に落ちる
			if (transform->position.y < 1080 / 2) {
				SetMovePos(transform->position, { transform->position.x, transform->scale.y / 2, 0 });
			}
			//下に落ちる
			else {
				SetMovePos(transform->position, { transform->position.x, GE::Window::GetWindowSize().y - transform->scale.y / 2, 0 });
			}
			moveTimer = 0;
			enemyState = EnemyState::FALLING;
		}
		else if (enemyState == EnemyState::WALKING)
		{
			//デバッグ用　状態遷移
			SetMovePos(transform->position, *pBossPosition);
			moveTimer = 0;
			enemyState = EnemyState::DEADING;
		}
	}

}
void NormalEnemyComponent::UpdateTimer(float deltaTime)
{
	moveTimer += deltaTime;

	flyingLoopTimer += deltaTime;
	if (flyingLoopTimer >= MAX_FLYING_LOOP_TIMER) {
		flyingLoopTimer -= MAX_FLYING_LOOP_TIMER;
	}

	walkingLoopTimer += deltaTime;
	if (walkingLoopTimer >= MAX_WALK_LOOP_TIMER) {
		walkingLoopTimer -= MAX_WALK_LOOP_TIMER;

		if (enemyState == EnemyState::WALKING) {
			//プレイヤーと同じ側にいるなら動く
			if (pPlayerMoveEntity->GetStanceState() == stanceState) {
				//次の移動地点決定
				//右へ
				if (pPlayerPos->x - transform->position.x > 0) {
					SetMovePos(transform->position, transform->position + GE::Math::Vector3(WALK_SPEED, 0, 0));
				}
				//左へ
				else if (pPlayerPos->x - transform->position.x < 0) {
					SetMovePos(transform->position, transform->position - GE::Math::Vector3(WALK_SPEED, 0, 0));
				}
			}
			else {
				//動かない
				SetMovePos(transform->position, transform->position);
			}
		}
	}

	flyingAnimeLoopTimer += (deltaTime * FLYING_ANIME_LOOP_TIMER_SPEED);
	if (flyingAnimeLoopTimer >= MAX_FLYING_ANIME_LOOP_TIMER) {
		flyingAnimeLoopTimer -= MAX_FLYING_ANIME_LOOP_TIMER;
	}

	//歩いていないときは0をかけてタイマーストップさせる
	float filter = moveBeforePos.x == moveAfterPos.x ? 0 : 1;
	walkingAnimeLoopTimer += (deltaTime * WALKING_ANIME_LOOP_TIMER_SPEED * filter);
	if (walkingAnimeLoopTimer >= MAX_WALKING_ANIME_LOOP_TIMER) {
		walkingAnimeLoopTimer -= MAX_WALKING_ANIME_LOOP_TIMER;
	}
}

void NormalEnemyComponent::CheckStance()
{
	if (transform->position.y <= 1080 / 2) {
		stanceState = StanceState::INVERSE;
	}
	else {
		stanceState = StanceState::NORMAL;
	}
}

void NormalEnemyComponent::UpdateGenerating()
{
	//イージングかけてBossの座標からあらかじめ決めてある位置に移動
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseOutQuart(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseOutQuart(t);

	transform->position = { x, y, 0 };

	//移動終わったら状態遷移
	if (moveTimer >= 1.0f) {
		enemyState = EnemyState::FLYING;
	}
}

void NormalEnemyComponent::UpdateFlying()
{
	//上下にふわふわ浮く感じ
	const float amount = 20;
	transform->position = moveAfterPos + GE::Math::Vector3{ 0, amount * sinf((flyingLoopTimer / 2.0f) * 360 * GE::Math::PI / 180), 0 };

	////デバッグ用　状態遷移
	//if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F1)) {
	//	//上に落ちる
	//	if (transform->position.y < 1080 / 2) {
	//		SetMovePos(transform->position, { transform->position.x, transform->scale.y, 0 });
	//	}
	//	//下に落ちる
	//	else {
	//		SetMovePos(transform->position, { transform->position.x, 1080 - transform->scale.y, 0 });
	//	}
	//	moveTimer = 0;
	//	enemyState = EnemyState::FALLING;
	//}
}

void NormalEnemyComponent::UpdateFalling()
{
	//イージングかけてBossの座標からあらかじめ決めてある位置に移動
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseInQuart(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseInQuart(t);

	transform->position = { x, y, 0 };

	//移動終わったら状態遷移
	if (moveTimer >= 1.0f) {
		//タイマーリセット (ばらつき出す)
		walkingLoopTimer = MAX_WALK_LOOP_TIMER * 3 / 4 + GE::RandomMaker::GetFloat(0, MAX_WALK_LOOP_TIMER / 4);
		//次のタイマーループまで動かない
		SetMovePos(transform->position, transform->position);

		enemyState = EnemyState::WALKING;
	}
}

void NormalEnemyComponent::UpdateWalking()
{
	//プレイヤーに向かって歩く
	//進行方向に身体をのばす
	if (walkingLoopTimer < MAX_WALK_LOOP_TIMER / 2) {

		float t = walkingLoopTimer / (MAX_WALK_LOOP_TIMER / 2);
		float posX = moveBeforePos.x + ((moveAfterPos.x - moveBeforePos.x) / 2) * GE::Math::Easing::EaseOutQuart(t);
		float posY = moveBeforePos.y;
		float scaleX = INIT_SCALE;
		float scaleY = INIT_SCALE;

		//移動するならy座標とスケールも変化
		if (moveBeforePos.x != moveAfterPos.x) {
			if (stanceState == StanceState::NORMAL){
				posY += INIT_SCALE / 8 * GE::Math::Easing::EaseOutQuart(t);
			}
			else {
				posY -= INIT_SCALE / 8 * GE::Math::Easing::EaseOutQuart(t);
			}
			scaleX = INIT_SCALE + WALK_SPEED * GE::Math::Easing::EaseOutQuart(t);
			scaleY = INIT_SCALE - INIT_SCALE / 4 * GE::Math::Easing::EaseOutQuart(t);
		}

		transform->position.x = posX;
		transform->position.y = posY;
		transform->scale.x = scaleX;
		transform->scale.y = scaleY;
	}
	//身体をちぢめる
	else {
		float t = (walkingLoopTimer - MAX_WALK_LOOP_TIMER / 2) / (MAX_WALK_LOOP_TIMER / 2);
		float posX = moveBeforePos.x + ((moveAfterPos.x - moveBeforePos.x) / 2) + ((moveAfterPos.x - moveBeforePos.x) / 2) * GE::Math::Easing::EaseOutQuart(t);
		float posY = moveBeforePos.y;
		float scaleX = INIT_SCALE;
		float scaleY = INIT_SCALE;

		//移動するならスケールも変化
		if (moveBeforePos.x != moveAfterPos.x) {
			if (stanceState == StanceState::NORMAL) {
				posY = (posY + INIT_SCALE / 8) - INIT_SCALE / 8 * GE::Math::Easing::EaseOutQuart(t);
			}
			else {
				posY = (posY - INIT_SCALE / 8) + INIT_SCALE / 8 * GE::Math::Easing::EaseOutQuart(t);
			}
			scaleX = (INIT_SCALE + WALK_SPEED) - WALK_SPEED * GE::Math::Easing::EaseOutQuart(t);
			scaleY = INIT_SCALE * 3 / 4 + INIT_SCALE / 4 * GE::Math::Easing::EaseOutQuart(t);
		}

		transform->position.x = posX;
		transform->position.y = posY;
		transform->scale.x = scaleX;
		transform->scale.y = scaleY;
	}
	
	//プレイヤーが自身と同じ側に来たらタイマーリセットして動き出すようにする
	static StanceState prevPlayerStanceState = pPlayerMoveEntity->GetStanceState();
	if (pPlayerMoveEntity->GetStanceState() != prevPlayerStanceState &&
		pPlayerMoveEntity->GetStanceState() == stanceState) {
		//タイマーリセット (ばらつき出す)
		walkingLoopTimer = MAX_WALK_LOOP_TIMER * 3 / 4 + GE::RandomMaker::GetFloat(0, MAX_WALK_LOOP_TIMER / 4);
		//次のタイマーループまで動かない
		SetMovePos(transform->position, transform->position);
	}
	prevPlayerStanceState = pPlayerMoveEntity->GetStanceState();


	////デバッグ用　状態遷移
	//if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F3)) {
	//	SetMovePos(transform->position, *pBossPosition);
	//	moveTimer = 0;
	//	enemyState = EnemyState::DEADING;
	//}
}

void NormalEnemyComponent::UpdateDeading()
{
	//イージングかけてBossの座標からあらかじめ決めてある位置に移動
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseInBack(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseInBack(t);

	transform->position = { x, y, 0 };

	//移動終わったら死ぬ
	if (moveTimer >= 1.0f) {
		enemyState = EnemyState::DEAD;
	}
}

void NormalEnemyComponent::SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after)
{
	moveBeforePos = before;
	moveAfterPos = after;
}
