#include "BossEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GameFramework/Component/BoxCollider.h>
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/GUI\GUIManager.h        >
#include "NormalEnemyComponent.h"
#include "CollisionManager.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"
#include "Tutorial.h"
#include "GameUtility.h"

const GE::Math::Vector3 BossEnemyComponent::SPRITE_SIZE = { 512, 384, 0 };
const float BossEnemyComponent::MIN_SCALE = 0.5f;
const float BossEnemyComponent::MOVE_SPEED = 0.5f;

void BossEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2, 1080 / 2, 0 };

	Initialize();
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

	//�`���[�g���A����ԑJ�ڗp
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
	//�`���[�g���A���X�L�b�v���͍ő吶��������`���[�g���A����������
	if (Tutorial::IsSkipTutorial()) {
		maxGenerateCount -= 2;
	}

	//�������C�t
	life = maxGenerateCount;

	//�G�����J�E���g���Z�b�g
	generateCount = 0;

	//������Ԃ͓����Ȃ�
	velocity = 0;

	//�����͒ʏ�̑傫��
	scaleDownMag = 1.0f;

	//�^�C�}�[�͍ő�l�Ń��Z�b�g
	scaleDownTimer = 1;
}

void BossEnemyComponent::Move()
{
	//���C�t�������ȉ��ɂȂ����瓮���o��
	const int START_MOVE_LIFE = Tutorial::IsSkipTutorial() ? maxGenerateCount / 2 : (maxGenerateCount - 2) / 2;
	if (life <= START_MOVE_LIFE && velocity == 0) {
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
	float beforeScale = scaleDownMag + (1.0 - MIN_SCALE) / maxGenerateCount;
	float scaleMag = beforeScale + (scaleDownMag - beforeScale) * GE::Math::Easing::EaseOutQuart(t);

	//���g�̃X�P�[��������������
	transform->scale = SPRITE_SIZE * scaleMag;
}

void BossEnemyComponent::UpdateLife()
{
	//�G�l�~�[���X�g�������Ď���ł�����폜�A���C�t����
	for (int i = 0; i < normalEnemies.size();) {
		if (normalEnemies[i]->IsDead()) {
			life--;
			normalEnemies.erase(normalEnemies.begin() + i);
		}
		else {
			i++;
		}
	}

	//���C�t0�ȉ��Ń��U���g��
	if (life <= 0) {
		GameUtility::TimerStop();
		GameUtility::SetGameState(GameState::RESULT);
	}
}

void BossEnemyComponent::GenerateNormalEnemy()
{
	//�O�t���[���Ő����t���O�����ĂȂ�������return
	if (isGenerate == false) { return; }

	//���łɍő吶���񐔈ȏ�ł�������return
	if (generateCount >= maxGenerateCount) { return; }

	//�G�����ʒu�ݒ�
	GE::Math::Vector3 afterPos = {};

	//�`���[�g���A�����͌Œ���W�ɏo��������
	if ((int)Tutorial::GetTutorialState() == (int)TutorialState::FIRST_ATTACK + 1) {
		afterPos = Tutorial::POS_GENERATE_ENEMY_1;
	}
	else if ((int)Tutorial::GetTutorialState() == (int)TutorialState::SECOND_ATTACK + 1) {
		afterPos = Tutorial::POS_GENERATE_ENEMY_2;
	}
	else {
		//�{�X�ɂ��������d�Ȃ�Ȃ��͈͂Ń����_���Ɉړ����W����
		float x = GE::RandomMaker::GetFloat(1920 / 4, 1920 / 4 + 1920 / 2);

		float y = GE::RandomMaker::GetFloat(100, 300);
		if (GE::RandomMaker::GetInt(0, 1) == 1) {
			y *= -1;
		}
		afterPos = { x, 1080 / 2 + y, 0 };
	}
	

	//NormalEnemy����
	auto* newEnemy = pGameObjectManager->AddGameObject(new GE::GameObject());
	newEnemy->SetName("NormalEnemy_" + std::to_string(normalEnemies.size()));
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
	scaleDownMag -= (1.0 - MIN_SCALE) / maxGenerateCount;

	//�^�C�}�[���Z�b�g
	scaleDownTimer = 0;

	//���������₷
	generateCount++;

	isGenerate = false;
}
