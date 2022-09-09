#include "BossEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GameFramework/Component/BoxCollider.h>
#include <GatesEngine/Header/Util/Random.h           >
#include "NormalEnemyComponent.h"
#include "CollisionManager.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"

const GE::Math::Vector3 BossEnemyComponent::SPRITE_SIZE = { 512, 384, 0 };
const float BossEnemyComponent::MIN_SCALE = 0.5f;
const float BossEnemyComponent::MOVE_SPEED = 0.5f;

void BossEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2, 1080 / 2, 0 };

	//�������C�t�͍ő�G������
	life = MAX_GENERATE_COUNT;

	//������Ԃ͓����Ȃ�
	velocity = 0;

	//�����͒ʏ�̑傫��
	scaleDownMag = 1.0f;
}

void BossEnemyComponent::Update(float deltaTime)
{
	beforeIsHitPlayer = isHitPlayer;
	if (HitStopManager::GetInstance()->IsActive() == false)
	{
		// �O��̃t���[���̃v���C���[�Ƃ̃q�b�g�󋵂�ێ�
		isHitPlayer = false;
	}

	//�^�C�}�[�X�V
	UpdateTimer(deltaTime);

	//�X�P�[���X�V
	UpdateScale();

	//���C�t�X�V
	UpdateLife();

	//�ړ�����
	Move();

	//�f�o�b�O�p�@�ق�Ƃ͍U�����ꂽ��Ăяo��
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F1)) {
		isGenerate = true;
	}
}

void BossEnemyComponent::LateDraw()
{
	//����ł���Ȃ�`�悵�Ȃ�
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

	// �O��̃t���[���Ńq�b�g���Ă���Ȃ�q�b�g��������Ȃ�
	if (beforeIsHitPlayer == true)
	{
		isHitPlayer = true;
		return;
	}

	isGenerate = true;
	isHitPlayer = true;
	HitStopManager::GetInstance()->Active(0.5f);
}

void BossEnemyComponent::Move()
{
	//���C�t�����l���ቺ�����瓮���o��
	if (life <= MAX_GENERATE_COUNT / 2 && velocity == 0) {
		velocity = (float)(GE::RandomMaker::GetInt(0, 1) * 2 - 1);
	}

	//���ړ�
	transform->position.x += velocity * MOVE_SPEED;

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

void BossEnemyComponent::UpdateTimer(float deltaTime)
{
	scaleDownTimer = scaleDownTimer + deltaTime < 1.0f ? scaleDownTimer + deltaTime : 1.0f;
}

void BossEnemyComponent::UpdateScale()
{
	//�C�[�W���O�����ăX�P�[���{���ω�
	float t = scaleDownTimer;
	float beforeScale = scaleDownMag + (1.0 - MIN_SCALE) / MAX_GENERATE_COUNT;
	float scaleMag = beforeScale + (scaleDownMag - beforeScale) * GE::Math::Easing::EaseOutQuart(t);

	//���g�̃X�P�[��������������
	transform->scale = SPRITE_SIZE * scaleMag;
}

void BossEnemyComponent::UpdateLife()
{
	//�G�l�~�[���X�g�������Ď���ł���̂̐��������C�t�l����������
	//(�R�X�g���߂Ȃ̂ŏ������������ق�����������)
	life = MAX_GENERATE_COUNT;
	for (auto& v : normalEnemies) {
		if (v->IsDead()) {
			life--;
		}
	}
}

void BossEnemyComponent::GenerateNormalEnemy()
{
	//�O�t���[���Ő����t���O�����ĂȂ�������return
	if (isGenerate == false) { return; }

	//���łɍő吶���񐔈ȏ�ł�������return
	if (normalEnemies.size() >= MAX_GENERATE_COUNT) { return; }

	//�����_���ňړ����W����
	float x = GE::RandomMaker::GetFloat(1920 / 4, 1920 / 4 + 1920 / 2);

	float y = GE::RandomMaker::GetFloat(100, 300);
	if (GE::RandomMaker::GetInt(0, 1) == 1) {
		y *= -1;
	}

	GE::Math::Vector3 afterPos = { x, 1080 / 2 + y, 0 };

	//NormalEnemy����
	auto* newEnemy = pGameObjectManager->AddGameObject(new GE::GameObject());
	newEnemy->SetName("test_enemy_" + std::to_string(normalEnemies.size()));
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

	//�X�P�[���l������
	scaleDownMag -= (1.0 - MIN_SCALE) / MAX_GENERATE_COUNT;

	//�^�C�}�[���Z�b�g
	scaleDownTimer = 0;

	isGenerate = false;
}
