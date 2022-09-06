#include "NormalEnemyComponent.h"
#include <GatesEngine/Header/Graphics/Window.h>

void NormalEnemyComponent::Start()
{
	const float SPRITE_SIZE = 100;

	transform->scale = SPRITE_SIZE;
	transform->position = generateAfterPos;
}

void NormalEnemyComponent::Update(float deltaTime)
{
	//ó‘Ô‚É‰ž‚¶‚Äˆ—‚ð•ª‚¯‚é
	if (enemyState == EnemyState::Generating) {
		MoveGenerating();
	}
	else if (enemyState == EnemyState::Flying) {
		MoveFlying();
	}
	else if (enemyState == EnemyState::Falling) {
		MoveFalling();
	}
	else {
		MoveWalking();
	}
}

void NormalEnemyComponent::LateDraw()
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
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("normal_enemy")->GetSRVNumber() });
	graphicsDevice->DrawMesh("2DPlane");
}

void NormalEnemyComponent::MoveGenerating()
{

}

void NormalEnemyComponent::MoveFlying()
{
}

void NormalEnemyComponent::MoveFalling()
{
}

void NormalEnemyComponent::MoveWalking()
{
}

void NormalEnemyComponent::SetGeneratePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after)
{
	generateBeforePos = before;
	generateAfterPos = after;
}
