#include "BossEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GameFramework/Component/BoxCollider.h>
#include "NormalEnemyComponent.h"

void BossEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	const float SPRITE_SIZE = 100;

	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2, 1080 / 2, 0 };

	velocity = 1;
	//������Ԃ͓����Ȃ�
	moveSpeed = 0;
}

void BossEnemyComponent::Update(float deltaTime)
{
	Move();

	//�f�o�b�O�p�@�ق�Ƃ͍U�����ꂽ��Ăяo��
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::SPACE)) {
		isGenerate = true;
	}
}

void BossEnemyComponent::LateDraw()
{
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

void BossEnemyComponent::Move()
{
	//���ړ�
	transform->position.x += velocity * moveSpeed;

	//�����]��
	if (transform->position.x < transform->scale.x / 2) {
		transform->position.x = transform->scale.x / 2;
		velocity *= -1;
	}
	else if (transform->position.x > 1920 - transform->scale.x / 2) {
		transform->position.x = 1920 - transform->scale.x / 2;
		velocity *= -1;
	}
}

void BossEnemyComponent::GenerateNormalEnemy()
{
	if (isGenerate == false) { return; }

	auto* testObject = pGameObjectManager->AddGameObject(new GE::GameObject());
	testObject->SetName("test_enemy");
	testObject->GetTransform()->position = { 0,0,0 };
	testObject->SetDrawAxisEnabled(true);
	auto* sampleCollider = testObject->AddComponent<GE::BoxCollider>();
	auto* normalEnemyComponent = testObject->AddComponent<NormalEnemyComponent>();
	normalEnemyComponent->SetGeneratePos(transform->position, { 1920 / 2, 0, 0 });	//�Ƃ肠�������ߑł�
	sampleCollider->SetCenter({ 0,0,0 });
	sampleCollider->SetSize({ 2 });
	sampleCollider->SetType(GE::ColliderType::OBB);

	testObject->Awake();

	isGenerate = false;
}
