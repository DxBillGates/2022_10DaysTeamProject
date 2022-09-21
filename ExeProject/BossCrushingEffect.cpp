#include "BossCrushingEffect.h"
#include <GatesEngine/Header/Util/Math/Easing.h>

void BossCrushingEffect::Initialize()
{
	Effect::Initialize();

	size = 0;
}

void BossCrushingEffect::Update(float deltaTime)
{
	const GE::Math::Vector3 START_SIZE = { 100,100,0 };
	const GE::Math::Vector3 END_SIZE = { 1000,1000,0 };

	float lerpTime = isActive.GetTime()/* / isActive.GetMaxTimeProperty()*/;
	size = GE::Math::Vector3::Lerp(START_SIZE, END_SIZE, GE::Math::Easing::EaseInExpo(lerpTime));

	Effect::Update(deltaTime);
}

void BossCrushingEffect::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(size * originScale);
	modelMatrix *= GE::Math::Matrix4x4::Translate(originPosition);

	GE::Material material;
	material.color = GE::Color(1,0,1,1);

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get("dotEffectTexture")->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}

void BossCrushingEffect::Active(const GE::Math::Vector3& position, float scale)
{
	Effect::Active(position,scale);
	isActive.SetMaxTimeProperty(EFFECT_TIME);

}
