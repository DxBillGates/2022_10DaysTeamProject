#include "SlashEffect.h"
#include "Camera2D.h"
#include "GameSetting.h"
#include <GatesEngine/Header/Util/Utility.h>
#include <GatesEngine/Header/Util/Math/Easing.h>
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/Util/Random.h>
#include <cmath>

void SlashEffect::Initialize()
{
	size = { 100,1000,0 };
	angle = 0;

	Effect::Initialize();
}

void SlashEffect::Update(float deltaTime)
{
	if (isActive.GetFlag() == false)return;

	const GE::Math::Vector3 START_SIZE = { 1000,100,0 };
	const GE::Math::Vector3 END_SIZE = { 200,2000,0 };

	float lerpTime = isActive.GetTime() / isActive.GetMaxTimeProperty();
	size = GE::Math::Vector3::Lerp(START_SIZE, END_SIZE, GE::Math::Easing::EaseInOutCirc(lerpTime));

	//GE::Utility::Printf("%3.3f\n", isActive.GetTime());

	Effect::Update(deltaTime);
}

void SlashEffect::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(size * originScale);
	modelMatrix *= GE::Math::Matrix4x4::RotationZ(angle);
	modelMatrix *= GE::Math::Matrix4x4::Translate(originPosition);

	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &Camera2D::GetInstance()->GetCameraInfo(), sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("slashEffectTexture")->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}

void SlashEffect::Active(const GE::Math::Vector3& position, float scale)
{
	Effect::Active(position,scale);
	isActive.SetMaxTimeProperty(EFFECT_TIME);

	angle = std::atan2f(GE::RandomMaker::GetFloat(-1, 1), GE::RandomMaker::GetFloat(-1, 1));
}