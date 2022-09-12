#include "DotExplosionEffect.h"
#include <GatesEngine/Header/Graphics/Window.h>
#include <GatesEngine/Header/Util/Math/Easing.h>

void DotExplosionEffect::Initialize()
{
	size = { 0,0,0 };

	Effect::Initialize();
}

void DotExplosionEffect::Update(float deltaTime)
{
	if (isActive.GetFlag() == false)return;

	const GE::Math::Vector3 START_SIZE = { 100,100,0 };
	const GE::Math::Vector3 END_SIZE = { 500,500,0 };

	float lerpTime = isActive.GetTime() / isActive.GetMaxTimeProperty();
	size = GE::Math::Vector3::Lerp(START_SIZE, END_SIZE, GE::Math::Easing::EaseInOutCirc(lerpTime));

	Effect::Update(deltaTime);
}

void DotExplosionEffect::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(size * originScale);
	modelMatrix *= GE::Math::Matrix4x4::Translate(originPosition);

	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("dotEffectTexture")->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}

void DotExplosionEffect::Active(const GE::Math::Vector3& position, float scale)
{
	Effect::Active(position, scale);
	isActive.SetMaxTimeProperty(EFFECT_TIME);
}
