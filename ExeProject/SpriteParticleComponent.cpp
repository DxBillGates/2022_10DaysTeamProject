#include "SpriteParticleComponent.h"
#include "Camera2D.h"

#include <GatesEngine/Header/Graphics/Window.h>

void SpriteParticleComponent::Start()
{
	moveTimer = 0;
	animeTimer = 0;
}

void SpriteParticleComponent::Update(float deltaTime)
{
	UpdateDelayTimer(deltaTime);

	//タイマー更新
	UpdateTimer(deltaTime);

	//位置更新
	UpdatePos();
}

void SpriteParticleComponent::LateDraw()
{
	//死んだら描画しない
	if (isStart == false) { return; }

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
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &Camera2D::GetInstance()->GetCameraInfo(), sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2, &material,sizeof(GE::Material)) });

	int drawNum = animeTimer / texChangeSpeed <= textureNum - 1 ? animeTimer / texChangeSpeed : textureNum - 1;
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get(textureName + "_" + std::to_string(drawNum))->GetSRVNumber()});

	graphicsDevice->DrawMesh("2DPlane");
}

void SpriteParticleComponent::Initialize()
{
	textureName = "NotSetTexture";
	textureNum = 1;
	texChangeSpeed = 0.1f;
	lifeTime = 1.0f;

	initPosition = {};
	velocity = {};
	accel = {};

	isStart = false;
	isLoopAnime = false;

	moveTimer = 0;
	animeTimer = 0;
}

void SpriteParticleComponent::StartAnime(bool isDelay, float delayTime)
{
	moveTimer = 0;
	animeTimer = 0;

	transform->position = initPosition;

	isStart = true;

	isStartDelay = isDelay;
	startDelay = delayTime;
}

void SpriteParticleComponent::UpdateTimer(float deltaTime)
{
	if (isStartDelay == true)return;

	moveTimer += deltaTime;

	//描画時間を過ぎたら停止
	if (moveTimer >= lifeTime) {
		isStart = false;
	}

	animeTimer += deltaTime;

	//連番枚数分タイマーが進んだ
	if (animeTimer >= texChangeSpeed * textureNum) {
		//ループするならタイマー停止しない
		if (isLoopAnime){
			animeTimer -= (texChangeSpeed * textureNum);
		}
		else {
			animeTimer = texChangeSpeed * textureNum;
		}
	}
}

void SpriteParticleComponent::UpdatePos()
{
	if (isStart == false || isStartDelay == true) { return; }

	velocity += accel;
	transform->position += velocity;
}

void SpriteParticleComponent::UpdateDelayTimer(float deltaTime)
{
	if (isStartDelay == false)return;

	if (delayTimer > startDelay) 
	{
		isStartDelay = false;
		startDelay = 0;
		isStart = true;
		return;
	}

	delayTimer += deltaTime;
}
