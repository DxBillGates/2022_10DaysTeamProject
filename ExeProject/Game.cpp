#include "Game.h"
#include "SampleScene.h"
#include "GameSetting.h"

#include <GatesEngine/Header/Graphics/Texture.h>
#include <GatesEngine/Header/Util/Random.h>
#include <GatesEngine/Header/Util/Utility.h>
#include <GatesEngine/Header/Graphics/CBufferStruct.h>
#include <GatesEngine/Header/GUI/GUIManager.h>

Game::Game()
	: Application()
{
}

Game::Game(const GE::WindowData& windowData, const GE::Math::Vector2& resolution)
	: Application(windowData, resolution)
{
}

Game::Game(const GE::Math::Vector2& size, const GE::Math::Vector2& resolution, const std::string& title, GE::WindowMode mode)
	: Application(size, resolution, title, mode)
{
}

Game::~Game()
{
}

bool Game::LoadContents()
{
	Application::LoadContents();

	//���\�[�X�ǉ��͂������火
	GE::Texture* playerTexture = new GE::Texture();
	playerTexture->Load("player.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(playerTexture, "texture_player");

	//�e�X�g�e�N�X�`��
	GE::Texture* bossEnemyTexture = new GE::Texture();
	bossEnemyTexture->Load("boss_enemy.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(bossEnemyTexture, "boss_enemy");

	//���ł���G�e�N�X�`��0
	GE::Texture* normalEnemyFlyingTex_0 = new GE::Texture();
	normalEnemyFlyingTex_0->Load("normal_enemy_flying_0.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingTex_0, "normal_enemy_flying_0");

	//���ł���G�e�N�X�`��1
	GE::Texture* normalEnemyFlyingTex_1 = new GE::Texture();
	normalEnemyFlyingTex_1->Load("normal_enemy_flying_1.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingTex_1, "normal_enemy_flying_1");

	//���ł���G�e�N�X�`��2
	GE::Texture* normalEnemyFlyingTex_2 = new GE::Texture();
	normalEnemyFlyingTex_2->Load("normal_enemy_flying_2.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingTex_2, "normal_enemy_flying_2");

	//���ł���G�̃_���[�W�e�N�X�`��
	GE::Texture* normalEnemyFlyingDamageTex = new GE::Texture();
	normalEnemyFlyingDamageTex->Load("normal_enemy_flying_damage.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingDamageTex, "normal_enemy_flying_damage");

	//�����Ă���G�e�N�X�`��
	GE::Texture* normalEnemyWalkingTex = new GE::Texture();
	normalEnemyWalkingTex->Load("normal_enemy_walking.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyWalkingTex, "normal_enemy_walking");

	//�����Ă���G�̃_���[�W�e�N�X�`��
	GE::Texture* normalEnemyWalkingDamageTex = new GE::Texture();
	normalEnemyWalkingDamageTex->Load("normal_enemy_walking_damage.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyWalkingDamageTex, "normal_enemy_walking_damage");

	auto* testScene = sceneManager.AddScene(new SampleScene("SampleScene"));
	sceneManager.ChangeScene("SampleScene");

	return true;
}

bool Game::Initialize()
{
	Application::Initialize();

	GameSetting::GetInstance()->Initialize();

	return true;
}

bool Game::Update()
{
	GE::GUIManager::StartFrame();
	GameSetting::GetInstance()->ChangingGameTime(timer.GetElapsedTime());
	Application::Update();
	return true;
}

bool Game::Draw()
{
	GE::ICBufferAllocater* cbufferAllocater = graphicsDevice.GetCBufferAllocater();
	GE::RenderQueue* renderQueue = graphicsDevice.GetRenderQueue();

	graphicsDevice.ClearDefaultRenderTarget(GE::Color::Blue());
	//graphicsDevice.SetDefaultRenderTarget();

	graphicsDevice.ClearLayer("resultLayer");
	graphicsDevice.SetLayer("resultLayer");

	graphicsDevice.SetShaderResourceDescriptorHeap();
	graphicsDevice.ResetCBufferAllocater();
	graphicsDevice.SetShader("DefaultMeshShader");

	{
		GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale({ 25 });
		const GE::CameraInfo& cameraInfo = mainCamera->GetCameraInfo();
		GE::Material material;
		GE::DirectionalLightInfo directionalLight;

		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
		renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2, &material, sizeof(GE::Material)) });
		renderQueue->AddSetConstantBufferInfo({ 3,cbufferAllocater->BindAndAttachData(3, &directionalLight, sizeof(GE::DirectionalLightInfo)) });
		graphicsDevice.DrawMesh("Sphere");

		graphicsDevice.SetShader("DefaultMeshWithTextureShader");
		modelMatrix = GE::Math::Matrix4x4::Identity();
		modelMatrix *= GE::Math::Matrix4x4::Scale({ 50 });
		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		renderQueue->AddSetShaderResource({ 4,graphicsDevice.GetTextureManager()->Get("texture_null")->GetSRVNumber() });
		graphicsDevice.DrawMesh("Skydome");


		graphicsDevice.SetShader("DefaultLineShader");
		modelMatrix = GE::Math::Matrix4x4::Identity();
		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		graphicsDevice.DrawMesh("Grid");

		Application::Draw();
	}

	graphicsDevice.ExecuteRenderQueue();
	graphicsDevice.ExecuteCommands();

	graphicsDevice.SetShaderResourceDescriptorHeap();
	graphicsDevice.SetDefaultRenderTarget();
	graphicsDevice.SetShader("DefaultSpriteWithTextureShader");
	auto windowSize = GE::Window::GetWindowSize();
	GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale({ windowSize.x,windowSize.y,0 });
	windowSize.x /= 2;
	windowSize.y /= 2;
	modelMatrix *= GE::Math::Matrix4x4::Translate({ windowSize.x,windowSize.y,0 });
	GE::Material material;
	material.color = GE::Color::White();

	GE::CameraInfo cameraInfo;
	cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,0,0 }, { 0,0,1 }, { 0,1,0 });
	cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice.GetLayerManager()->Get("resultLayer")->GetRenderTexture()->GetSRVNumber() });
	graphicsDevice.DrawMesh("2DPlane");

	graphicsDevice.ExecuteRenderQueue();
	GE::GUIManager::EndFrame();
	graphicsDevice.ExecuteCommands();
	graphicsDevice.ScreenFlip();
	return true;
}
