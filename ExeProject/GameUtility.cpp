#include "GameUtility.h"
#include "MonitorEffect.h"
#include "Tutorial.h"
#include <GatesEngine/Header/GUI/GUIManager.h>

float GameUtility::timer = 0;
bool GameUtility::isStartTimer = false;
int GameUtility::nowChain = 0;
bool GameUtility::isPlaySE = true;
GameState GameUtility::gameState = GameState::GAME;

GE::IGraphicsDeviceDx12* GameUtility::graphicsDevice = nullptr;

const GE::Math::Vector3 SCALE_TITLE = GE::Math::Vector3(804, 192, 0) * 0.6;
const GE::Math::Vector3 SCALE_START = GE::Math::Vector3(192, 64, 0);

const GE::Math::Vector3 POS_TITLE = GE::Math::Vector3(1430, 215, 0) + SCALE_TITLE / 2;
const GE::Math::Vector3 POS_START = GE::Math::Vector3(1428 + 486 / 2, 380, 0);


void GameUtility::Initialize()
{
	timer = 0;
	gameState = GameState::GAME;
}

void GameUtility::TimerStart()
{
	isStartTimer = true;
}

void GameUtility::UpdateTimer(float deltaTime)
{
	if (isStartTimer) { timer += deltaTime; }

	if (Tutorial::IsEndTutorial() && MonitorEffect::IsStart("Start") == false && timer >= 0.5f) {
		MonitorEffect::StartEffect("Start");
	}
}

void GameUtility::TimerStop()
{
	isStartTimer = false;
}

void GameUtility::OnGui()
{
#ifdef _DEBUG
	ImGui::Begin("Chain");
	ImGui::Text("Chain:%d", nowChain);
	ImGui::End();
#endif
}

void GameUtility::DrawBackground()
{
	//BG
	Draw({ 1920 / 2,1080 / 2,0 }, { 1920, 1080, 0 }, "Background");

	//Title
	if (MonitorEffect::IsDraw("Tutorial_Right") &&
		(gameState == GameState::GAME)) {
		Draw(POS_TITLE, SCALE_TITLE, "Title");
	}

	//Start
	float f = Tutorial::IsSkipTutorial() ? 0 : 0.5f;
	if (Tutorial::IsEndTutorial() && MonitorEffect::IsDraw("Start") && timer >= f && timer < 5.0f) {
		Draw(POS_START, SCALE_START * 1.5f, "Start");
	}

}

void GameUtility::Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name)
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice->GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice->GetRenderQueue();

	graphicsDevice->SetShader("DefaultSpriteWithTextureShader");

	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale(scale);

	modelMatrix *= GE::Math::Matrix4x4::Translate(pos);
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,0,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice->GetTextureManager()->Get(name)->GetSRVNumber() });

	graphicsDevice->DrawMesh("2DPlane");
}