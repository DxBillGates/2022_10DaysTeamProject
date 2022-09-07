#include "NormalEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GUI\GUIManager.h>

void NormalEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	const float SPRITE_SIZE = 100;

	transform->scale = SPRITE_SIZE;
	transform->position = moveBeforePos;

	moveTimer = 0;
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
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy")->GetSRVNumber() });
	graphicsDevice->DrawMesh("2DPlane");
}

void NormalEnemyComponent::UpdateTimer(float deltaTime)
{
	moveTimer += deltaTime;
	loopTimer += deltaTime;
	if (loopTimer >= 2.0f) {
		loopTimer -= 2.0f;
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
	transform->position = moveAfterPos + GE::Math::Vector3{ 0, amount * sinf((loopTimer / 2.0f) * 360 * GE::Math::PI / 180), 0 };

	//デバッグ用　状態遷移
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F1)) {
		//上に落ちる
		if (transform->position.y < 1080 / 2) {
			SetMovePos(transform->position, { transform->position.x, transform->scale.y, 0 });
		}
		//下に落ちる
		else {
			SetMovePos(transform->position, { transform->position.x, 1080 - transform->scale.y, 0 });
		}
		moveTimer = 0;
		enemyState = EnemyState::FALLING;
	}
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
		enemyState = EnemyState::WALKING;
	}
}

void NormalEnemyComponent::UpdateWalking()
{
	//デバッグ用　状態遷移
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F2)) {
		SetMovePos(transform->position, *pBossPosition);
		moveTimer = 0;
		enemyState = EnemyState::DEADING;
	}
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
