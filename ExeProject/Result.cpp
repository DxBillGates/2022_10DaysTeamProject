#include "Result.h"
#include "GameUtility.h"
#include <GatesEngine/Header/GUI/GUIManager.h>

float Result::timer = 0;
bool Result::isStartTimer = false;

GE::IGraphicsDeviceDx12* Result::graphicsDevice = nullptr;

const GE::Math::Vector3 SCALE_CLEAR_TIME = { 512, 96, 0 };
const GE::Math::Vector3 SCALE_NUMBER = { 32, 64, 0 };

const GE::Math::Vector3 POS_BASE_MONITOR_LEFT = { 12, 348, 0 };
const GE::Math::Vector3 POS_BASE_MONITOR_RIGHT = { 1428, 144, 0 };

const GE::Math::Vector3 POS_CLEAR_TIME = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(16, 0, 0) + SCALE_CLEAR_TIME / 2;
const GE::Math::Vector3 POS_NUMBER_LEFT = POS_BASE_MONITOR_LEFT + GE::Math::Vector3(486 / 2 - 32 * 7 / 2, 150, 0) + SCALE_NUMBER / 2;


void Result::Initialize()
{
	timer = 0;
	isStartTimer = false;
}

void Result::UpdateTimer(float deltaTime)
{
	if (GameUtility::GetGameState() == GameState::RESULT) {
		isStartTimer = true;
	}
	else {
		isStartTimer = false;
	}

	if (isStartTimer) { timer += deltaTime; }
}

void Result::Draw()
{
	if (timer >= 0.5f) {
		Draw(POS_CLEAR_TIME, SCALE_CLEAR_TIME, "ClearTime");
	}

	if (timer >= 1.0f) {
		std::string num = std::to_string(GameUtility::GetClearTime());
		DrawNum(num.substr(0, num.find(".") + 5), POS_NUMBER_LEFT, SCALE_NUMBER, 0);
	}
}

void Result::Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(pos);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,1,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get(name)->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}

void Result::DrawNum(const std::string& num, const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, float padding)
{
	for (int i = 0; i < num.size(); i++) {
		if (num[i] != '.') {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Number_" + num.substr(i, 1));
		}
		else {
			Draw(pos + GE::Math::Vector3(i * scale.x + padding, 0, 0), scale, "Dot");
		}
	}
}
