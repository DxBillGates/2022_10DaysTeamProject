#include "NormalEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/GUI\GUIManager.h>
#include <GatesEngine/Header/Util/Random.h>

const float NormalEnemyComponent::INIT_SCALE = 100;
const float NormalEnemyComponent::WALK_SPEED = INIT_SCALE;
const float NormalEnemyComponent::MAX_FLYING_LOOP_TIMER = 2.0f;
const float NormalEnemyComponent::MAX_WALK_LOOP_TIMER = 1.5f;

void NormalEnemyComponent::Start()
{
	inputDevice = GE::InputDevice::GetInstance();

	const float SPRITE_SIZE = INIT_SCALE;

	transform->scale = SPRITE_SIZE;
	transform->position = moveBeforePos;

	moveTimer = 0;
	flyingLoopTimer = 0;
	walkLoopTimer = 0;
}

void NormalEnemyComponent::Update(float deltaTime)
{
	//�^�C�}�[�X�V
	UpdateTimer(deltaTime);

	//�摜�̌����𐳂�
	CheckStance();

	//��Ԃɉ����ď����𕪂���
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
	//���񂾂�`�悵�Ȃ�
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
	flyingLoopTimer += deltaTime;
	if (flyingLoopTimer >= MAX_FLYING_LOOP_TIMER) {
		flyingLoopTimer -= MAX_FLYING_LOOP_TIMER;
	}
	walkLoopTimer += deltaTime;
	if (walkLoopTimer >= MAX_WALK_LOOP_TIMER) {
		walkLoopTimer -= MAX_WALK_LOOP_TIMER;

		if (enemyState == EnemyState::WALKING) {
			//�v���C���[�Ɠ������ɂ���Ȃ瓮��
			if (pPlayerMoveEntity->GetStanceState() == stanceState) {
				//���̈ړ��n�_����
				//�E��
				if (pPlayerPos->x - transform->position.x > 0) {
					SetMovePos(transform->position, transform->position + GE::Math::Vector3(WALK_SPEED, 0, 0));
				}
				//����
				else if (pPlayerPos->x - transform->position.x < 0) {
					SetMovePos(transform->position, transform->position - GE::Math::Vector3(WALK_SPEED, 0, 0));
				}
			}
			else {
				//�����Ȃ�
				SetMovePos(transform->position, transform->position);
			}
		}
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
	//�C�[�W���O������Boss�̍��W���炠�炩���ߌ��߂Ă���ʒu�Ɉړ�
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseOutQuart(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseOutQuart(t);

	transform->position = { x, y, 0 };

	//�ړ��I��������ԑJ��
	if (moveTimer >= 1.0f) {
		enemyState = EnemyState::FLYING;
	}
}

void NormalEnemyComponent::UpdateFlying()
{
	//�㉺�ɂӂ�ӂ핂������
	const float amount = 20;
	transform->position = moveAfterPos + GE::Math::Vector3{ 0, amount * sinf((flyingLoopTimer / 2.0f) * 360 * GE::Math::PI / 180), 0 };

	//�f�o�b�O�p�@��ԑJ��
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F2)) {
		//��ɗ�����
		if (transform->position.y < 1080 / 2) {
			SetMovePos(transform->position, { transform->position.x, transform->scale.y / 2, 0 });
		}
		//���ɗ�����
		else {
			SetMovePos(transform->position, { transform->position.x, GE::Window::GetWindowSize().y - transform->scale.y / 2, 0 });
		}
		moveTimer = 0;
		enemyState = EnemyState::FALLING;
	}
}

void NormalEnemyComponent::UpdateFalling()
{
	//�C�[�W���O������Boss�̍��W���炠�炩���ߌ��߂Ă���ʒu�Ɉړ�
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseInQuart(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseInQuart(t);

	transform->position = { x, y, 0 };

	//�ړ��I��������ԑJ��
	if (moveTimer >= 1.0f) {
		//�^�C�}�[���Z�b�g (�΂���o��)
		walkLoopTimer = MAX_WALK_LOOP_TIMER * 3 / 4 + GE::RandomMaker::GetFloat(0, MAX_WALK_LOOP_TIMER / 4);
		//���̃^�C�}�[���[�v�܂œ����Ȃ�
		SetMovePos(transform->position, transform->position);

		enemyState = EnemyState::WALKING;
	}
}

void NormalEnemyComponent::UpdateWalking()
{
	//�v���C���[�Ɍ������ĕ���
	//�i�s�����ɐg�̂��̂΂�
	if (walkLoopTimer < MAX_WALK_LOOP_TIMER / 2) {

		float t = walkLoopTimer / (MAX_WALK_LOOP_TIMER / 2);
		float posX = moveBeforePos.x + ((moveAfterPos.x - moveBeforePos.x) / 2) * GE::Math::Easing::EaseOutQuart(t);
		float scaleX = INIT_SCALE;

		//�ړ�����Ȃ�X�P�[�����ω�
		if (moveBeforePos.x != moveAfterPos.x) {
			scaleX = INIT_SCALE + WALK_SPEED * GE::Math::Easing::EaseOutQuart(t);
		}

		transform->position.x = posX;
		transform->scale.x = scaleX;
	}
	//�g�̂������߂�
	else {
		float t = (walkLoopTimer - MAX_WALK_LOOP_TIMER / 2) / (MAX_WALK_LOOP_TIMER / 2);
		float posX = moveBeforePos.x + ((moveAfterPos.x - moveBeforePos.x) / 2) + ((moveAfterPos.x - moveBeforePos.x) / 2) * GE::Math::Easing::EaseOutQuart(t);
		float scaleX = INIT_SCALE;
		
		//�ړ�����Ȃ�X�P�[�����ω�
		if (moveBeforePos.x != moveAfterPos.x) {
			scaleX = (INIT_SCALE + WALK_SPEED) - WALK_SPEED * GE::Math::Easing::EaseOutQuart(t);
		}

		transform->position.x = posX;
		transform->scale.x = scaleX;
	}
	
	//�v���C���[�����g�Ɠ������ɗ�����^�C�}�[���Z�b�g���ē����o���悤�ɂ���
	static StanceState prevPlayerStanceState = pPlayerMoveEntity->GetStanceState();
	if (pPlayerMoveEntity->GetStanceState() != prevPlayerStanceState &&
		pPlayerMoveEntity->GetStanceState() == stanceState) {
		//�^�C�}�[���Z�b�g (�΂���o��)
		walkLoopTimer = MAX_WALK_LOOP_TIMER * 3 / 4 + GE::RandomMaker::GetFloat(0, MAX_WALK_LOOP_TIMER / 4);
		//���̃^�C�}�[���[�v�܂œ����Ȃ�
		SetMovePos(transform->position, transform->position);
	}
	prevPlayerStanceState = pPlayerMoveEntity->GetStanceState();


	//�f�o�b�O�p�@��ԑJ��
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::F3)) {
		SetMovePos(transform->position, *pBossPosition);
		moveTimer = 0;
		enemyState = EnemyState::DEADING;
	}
}

void NormalEnemyComponent::UpdateDeading()
{
	//�C�[�W���O������Boss�̍��W���炠�炩���ߌ��߂Ă���ʒu�Ɉړ�
	float t = moveTimer <= 1.0f ? moveTimer : 1.0f;
	float x = moveBeforePos.x + (moveAfterPos.x - moveBeforePos.x) * GE::Math::Easing::EaseInBack(t);
	float y = moveBeforePos.y + (moveAfterPos.y - moveBeforePos.y) * GE::Math::Easing::EaseInBack(t);

	transform->position = { x, y, 0 };

	//�ړ��I������玀��
	if (moveTimer >= 1.0f) {
		enemyState = EnemyState::DEAD;
	}
}

void NormalEnemyComponent::SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after)
{
	moveBeforePos = before;
	moveAfterPos = after;
}
