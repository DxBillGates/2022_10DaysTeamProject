#include "PlayerComponent.h"
#include "GameSetting.h"
#include "HitStopManager.h"

#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

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

	transform->position = { 1920 / 2,GE::Window::GetWindowSize().y - transform->scale.y / 2,0 };

	const int MAX_HP = 3;
	hp = MAX_HP;
}

void PlayerComponent::Update(float deltaTime)
{
	const float GAME_TIME = GameSetting::GetInstance()->GetTime();

	// �ړ������̕ύX�e�X�g
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::A) && moveEntity.GetDirectionState() == MoveDirectionState::RIGHT
		|| inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::D) && moveEntity.GetDirectionState() == MoveDirectionState::LEFT)
	{
		moveEntity.ChangeMoveDirection();
	}



	const float MOVE_SPEED = 5;
	if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::A))
	{
		transform->position.x -= MOVE_SPEED * GAME_TIME;
	}
	if (inputDevice->GetKeyboard()->CheckHitKey(GE::Keys::D))
	{
		transform->position.x += MOVE_SPEED * GAME_TIME;
	}

	transform->position += knockbackVelocity * GAME_TIME;

	// �ړ��I�u�W�F�N�g�p�̊e��X�V����
	moveEntity.CheckTeleport(transform->position, transform->scale);
	moveEntity.UpdateChangeDirectionFlag(deltaTime, GAME_TIME);
	moveEntity.UpdateStanceAngle(deltaTime, GAME_TIME);


	if (invincibleFlag.GetOverTimeTrigger())
	{
		invincibleFlag.Initialize();
	}
	invincibleFlag.Update(deltaTime * GAME_TIME);

	UpdateKnockback(deltaTime * GAME_TIME);
}

void PlayerComponent::LateDraw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(transform->rotation);
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);
	transform->rotation = moveEntity.GetAngles();

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("texture_player")->GetSRVNumber() });

	//GE::TextureAnimationInfo animationInfo;
	//animationInfo.clipSize = 64;
	//animationInfo.pivot = { 1,1 };
	//renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });

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

	const float POWER = 3;
	const float KNOCKBACK_TIME = 0.5f;
	const float INVINCIBLE_TIME = 1.0f;

	setKnockbackVector = knockbackVelocity = GE::Math::Vector3::Normalize(transform->position - otherPosition) * POWER;
	setKnockbackVector.y = knockbackVelocity.y = 0;

	// �m�b�N�o�b�N�p�̃t���O������
	knockbackFlag.Initialize();
	knockbackFlag.SetMaxTimeProperty(KNOCKBACK_TIME);
	knockbackFlag.SetFlag(true);

	// ���G���ԗp�̃t���O������
	invincibleFlag.Initialize();
	invincibleFlag.SetMaxTimeProperty(INVINCIBLE_TIME);
	invincibleFlag.SetFlag(true);

	HitStopManager::GetInstance()->Active(0.5f);
}

void PlayerComponent::UpdateKnockback(float deltaTime)
{
	if (knockbackFlag.GetFlag() == false)return;

	if (knockbackFlag.GetOverTimeTrigger())
	{
		knockbackFlag.SetFlag(false);
		knockbackFlag.SetTime(knockbackFlag.GetMaxTimeProperty());
	}

	// �m�b�N�o�b�N�̌o�ߎ��Ԃ��擾�����`��ԂŃm�b�N�o�b�N�̃x�N�g��������������
	float lerpTime = knockbackFlag.GetTime() / knockbackFlag.GetMaxTimeProperty();
	knockbackVelocity = GE::Math::Vector3::Lerp(setKnockbackVector, GE::Math::Vector3(), lerpTime);

	knockbackFlag.Update(deltaTime);
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

		// 3��HP���v���C���[�̏�ŕ��U
		drawPosition = transform->position - GE::Math::Vector3(1, 0, 0) * SPACE * (i - 1);
		drawPosition -= GE::Math::Vector3(0, HEIGHT, 0) * (SPACE + DRAW_SIZE);
		modelMatrix *= GE::Math::Matrix4x4::Translate(drawPosition);

		// �`�悷��A�j���[�V�����ԍ��̎w��
		int drawAnimationNum = (i >= hp) ? 1 : 0;
		animationInfo.pivot.x = drawAnimationNum;

		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		renderQueue->AddSetConstantBufferInfo({ 5,cbufferAllocater->BindAndAttachData(5,&animationInfo,sizeof(GE::TextureAnimationInfo)) });
		graphicsDevice->DrawMesh("2DPlane");
	}
}

//void PlayerComponent::OnGui()
//{
//	float dragSpeed = 0.1f;
//	float maxValue = 100;
//	ImGui::DragFloat3("Angles", transform->rotation.value, dragSpeed, 0, 360);
//}
