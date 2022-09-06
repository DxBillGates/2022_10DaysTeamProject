#include "ShadowPlayerComponent.h"
#include <GatesEngine/Header/Graphics\CBufferStruct.h>
#include <GatesEngine/Header/Util/Utility.h          >
#include <GatesEngine/Header/Util/Random.h           >
#include <GatesEngine/Header/Graphics\Window.h       >
#include <GatesEngine/Header/GUI\GUIManager.h        >

void ShadowPlayerComponent::Start()
{
	moveEntity.Initialize();
	moveEntity.SetStanceState(StanceState::INVERSE);

	autoMove = true;

	const float SPRITE_SIZE = 100;
	transform->scale = SPRITE_SIZE;
	transform->position = { 1920 / 2,transform->scale.y / 2,0 };

}

void ShadowPlayerComponent::Update(float deltaTime)
{
	const float MOVE_SPEED = 7;

	if (autoMove == true)
	{
		if (moveEntity.GetDirectionState() == MoveDirectionState::RIGHT)
		{
			transform->position.x += MOVE_SPEED;
		}
		else
		{
			transform->position.x -= MOVE_SPEED;
		}
	}

	// 移動オブジェクト用の各種更新処理
	moveEntity.CheckTeleport(transform->position, transform->scale);
	moveEntity.UpdateChangeDirectionFlag(deltaTime, 1);
	moveEntity.UpdateStanceAngle(deltaTime, 1);
}

void ShadowPlayerComponent::LateDraw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(transform->scale);

	modelMatrix *= GE::Math::Matrix4x4::RotationZXY(moveEntity.GetAngles());
	//GE::Utility::Printf("%d,%d\n",(int)mousePos.x, (int)mousePos.y);

	modelMatrix *= GE::Math::Matrix4x4::Translate(transform->position);
	GE::Material material;
	material.color = GE::Color(0.5f, 0.5f, 0.5f, 1);
	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("texture_icon")->GetSRVNumber() });
	graphicsDevice->DrawMesh("2DPlane");
}

void ShadowPlayerComponent::OnGui()
{
	ImGui::Checkbox("flag", &autoMove);
}
