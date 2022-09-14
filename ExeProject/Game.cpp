#include "Game.h"
#include "SampleScene.h"
#include "GameSetting.h"
#include "MonitorEffect.h"

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

	//リソース追加はここから↓
	GE::Texture* playerTexture = new GE::Texture();
	playerTexture->Load("Player_stop.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(playerTexture, "texture_player");

	GE::Texture* playerTextureWalk = new GE::Texture();
	playerTextureWalk->Load("Player_walk.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(playerTextureWalk, "texture_player_walk");

	GE::Texture* playerTextureDead = new GE::Texture();
	playerTextureDead->Load("Player_Die.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(playerTextureDead, "texture_player_dead");


	GE::Texture* shadowPlayerTexture = new GE::Texture();
	shadowPlayerTexture->Load("Player_Shadow.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(shadowPlayerTexture, "Player_Shadow");

	//テストテクスチャ
	GE::Texture* bossEnemyTexture = new GE::Texture();
	bossEnemyTexture->Load("boss_enemy.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(bossEnemyTexture, "boss_enemy");

	for (int i = 0; i < 3; i++) {
		//飛んでいる敵テクスチャ
		GE::Texture* normalEnemyFlyingTex = new GE::Texture();
		normalEnemyFlyingTex->Load("normal_enemy/flying/" + std::to_string(i) + ".png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingTex, "normal_enemy_flying_" + std::to_string(i));
	}

	//飛んでいる敵のダメージテクスチャ
	GE::Texture* normalEnemyFlyingDamageTex = new GE::Texture();
	normalEnemyFlyingDamageTex->Load("normal_enemy/flying_damage.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyFlyingDamageTex, "normal_enemy_flying_damage");

	for (int i = 0; i < 3; i++) {
		//歩いている敵テクスチャ
		GE::Texture* normalEnemyWalkingTex = new GE::Texture();
		normalEnemyWalkingTex->Load("normal_enemy/walking/" + std::to_string(i) + ".png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(normalEnemyWalkingTex, "normal_enemy_walking_" + std::to_string(i));
	}

	//歩いている敵のダメージテクスチャ
	GE::Texture* normalEnemyWalkingDamageTex = new GE::Texture();
	normalEnemyWalkingDamageTex->Load("normal_enemy/walking_damage.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
	graphicsDevice.GetTextureManager()->Add(normalEnemyWalkingDamageTex, "normal_enemy_walking_damage");

	//爆発テクスチャ
	for (int i = 0; i < 12; i++) {
		//歩いている敵テクスチャ
		GE::Texture* explosionTex = new GE::Texture();
		explosionTex->Load("ex/" + std::to_string(i) + ".png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(explosionTex, "ex_" + std::to_string(i));
	}

	// 敵(飛んでる状態)
	{
		GE::Texture* enemyAnimationTexture = new GE::Texture();
		enemyAnimationTexture->Load("enemy/Enemy_air.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(enemyAnimationTexture, "FlyingEnemyAnimationTexture");
	}
	// 敵(落ちてる状態)
	{
		GE::Texture* enemyAnimationTexture = new GE::Texture();
		enemyAnimationTexture->Load("enemy/Enemy_walk.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(enemyAnimationTexture, "WalkingEnemyAnimationTexture");
	}
	// プレイヤーのHP
	{
		GE::Texture* playerHPTexture = new GE::Texture();
		playerHPTexture->Load("player_heart.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(playerHPTexture, "texture_player_heart");
	}

	{
		GE::Texture* slashEffectTexture = new GE::Texture();
		slashEffectTexture->Load("Effect/slash_effect.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(slashEffectTexture, "slashEffectTexture");
	}

	{
		GE::Texture* dotEffectTexture = new GE::Texture();
		dotEffectTexture->Load("Effect/Dot_Effect.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(dotEffectTexture, "dotEffectTexture");
	}

	{
		GE::Texture* explosionEffect = new GE::Texture();
		explosionEffect->Load("Effect/howa2.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(explosionEffect, "explosionEffectTexture");
	}

	//チュートリアル (TTRはtutorialの略)
	//「Tutorial」文字
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Tutorial.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Tutorial");
	}
	//Left
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Left.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Left");
	}
	//Right
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Right.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Right");
	}
	//Attack
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Attack.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Attack");
	}
	//Wait
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Wait.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Wait");
	}
	//Grid
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Tutorial/Grid.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "TTR_Grid");
	}

	//リザルト関連
	//Background
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Background.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Background");
	}

	//数字
	for (int i = 0; i < 10; i++)
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Number/" + std::to_string(i) + ".png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Number_" + std::to_string(i));
	}

	//「.」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Number/Dot.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Dot");
	}

	//「:」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Number/Colon.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Colon");
	}

	//「ClearTime」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("ClearTime.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "ClearTime");
	}

	//「WorldRanking」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("WorldRanking.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "WorldRanking");
	}

	//「NowLoading」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Now_Loading.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "NowLoading");
	}

	//「Load Failed」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("LoadFailed.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "LoadFailed");
	}

	//「GameOver」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("GameOver.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "GameOver");
	}

	//「Title」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("CrossSwitch.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Title");
	}

	//「Restart」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Restart.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Restart");
	}

	//「Exit」
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Exit.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Exit");
	}

	//カーソル
	{
		GE::Texture* texture = new GE::Texture();
		texture->Load("Cursol.png", graphicsDevice.GetDevice(), graphicsDevice.GetShaderResourceHeap());
		graphicsDevice.GetTextureManager()->Add(texture, "Cursol");
	}


	//効果音「HitNormal」
	{
		auto* audioData = audioManager.AddAudioData(new GE::AudioData("Resources/Audio/HitNormal.wav"), "HitNormalData");
		auto* audio = audioManager.AddAudio(new GE::Audio(audioData, "HitNormal"));
	}

	//効果音「HitBoss」
	{
		auto* audioData = audioManager.AddAudioData(new GE::AudioData("Resources/Audio/HitBossPlayer.wav"), "HitBossPlayerData");
		auto* audio = audioManager.AddAudio(new GE::Audio(audioData, "HitBoss"));
	}

	//効果音「Explosion」
	{
		auto* audioData = audioManager.AddAudioData(new GE::AudioData("Resources/Audio/Explosion07.wav"), "ExplosionData");
		auto* audio = audioManager.AddAudio(new GE::Audio(audioData, "Explosion"));
	}

	//効果音「Dash」
	{
		auto* audioData = audioManager.AddAudioData(new GE::AudioData("Resources/Audio/Dash.wav"), "DashData");
		auto* audio = audioManager.AddAudio(new GE::Audio(audioData, "Dash"));
	}

	//効果音「Player用Hit」
	{
		auto* audio = audioManager.AddAudio(new GE::Audio(audioManager.GetAudioData("HitBossPlayerData"), "PlayerHit"));
	}

	//効果音「CursolDecide」
	{
		auto* audioData = audioManager.AddAudioData(new GE::AudioData("Resources/Audio/CursolDecide.wav"), "CursolDecideData");
		auto* audio = audioManager.AddAudio(new GE::Audio(audioData, "CursolDecide"));
	}


	//モニターエフェクト用
	MonitorEffect::Add("Tutorial_Left");
	MonitorEffect::Add("Tutorial_Right");
	MonitorEffect::Add("ClearTimeText");
	MonitorEffect::Add("ClearTimeNum");
	MonitorEffect::Add("WorldRankingText");
	MonitorEffect::Add("WorldRankingNum");
	MonitorEffect::Add("LoadFailed");
	MonitorEffect::Add("Select");
	MonitorEffect::Add("GameOver");

	auto* testScene = sceneManager.AddScene(new SampleScene("SampleScene"));
	sceneManager.ChangeScene("SampleScene");

	return true;
}

bool Game::Initialize()
{
	Application::Initialize();

	GameSetting::GetInstance()->Initialize();

	float gaussValue = 2.5f;
	GE::Math::Vector2 right = { 1,0 };
	GE::Math::Vector2 up = { 0,1 };
	GE::Math::Vector2 windowSize = GE::Window::GetWindowSize();
	for (int i = 0; i < 6; ++i)
	{
		GE::Math::Vector2 vector = {};
		if (i % 2 == 0)
		{
			windowSize /= 2;
			vector = right;
		}
		else
		{
			vector = up;
		}

		GE::Math::SetGaussFilterData(windowSize, vector, gaussValue, &gaussFilterData[i], 0);

		gaussValue *= 2.0f;
	}

	timer.SetFrameRate(60);

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

	//graphicsDevice.SetShaderResourceDescriptorHeap();
	//graphicsDevice.SetDefaultRenderTarget();
	//graphicsDevice.SetShader("SpriteTextureForPostEffectShader");
	//auto windowSize = GE::Window::GetWindowSize();
	//GE::Math::Matrix4x4 modelMatrix = GE::Math::Matrix4x4::Scale({ windowSize.x,windowSize.y,0 });
	//windowSize.x /= 2;
	//windowSize.y /= 2;
	//modelMatrix *= GE::Math::Matrix4x4::Translate({ windowSize.x,windowSize.y,0 });
	//GE::Material material;
	//material.color = GE::Color::White();

	//GE::CameraInfo cameraInfo;
	//cameraInfo.viewMatrix = GE::Math::Matrix4x4::GetViewMatrixLookTo({ 0,0,0 }, { 0,0,1 }, { 0,1,0 });
	//cameraInfo.projMatrix = GE::Math::Matrix4x4::GetOrthographMatrix(GE::Window::GetWindowSize());

	//renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	//renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	//renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2,&material,sizeof(GE::Material)) });
	//renderQueue->AddSetShaderResource({ 4,graphicsDevice.GetLayerManager()->Get("resultLayer")->GetRenderTexture()->GetSRVNumber() });
	//graphicsDevice.DrawMesh("2DPlane");

	//graphicsDevice.ExecuteRenderQueue();
	//graphicsDevice.ExecuteCommands();

	// ここからポストエフェクト
	// ひとまず輝度抽出
	graphicsDevice.SetShaderResourceDescriptorHeap();
	graphicsDevice.ClearLayer("demoLayer");
	graphicsDevice.SetLayer("demoLayer");
	graphicsDevice.SetShader("BrightnessSamplingShader");

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
	renderQueue->AddSetShaderResource({ 5,graphicsDevice.GetLayerManager()->Get("EffectLayer")->GetRenderTexture()->GetSRVNumber() });

	const float BRIGHTNESS = 0.1f;
	renderQueue->AddSetConstantBufferInfo({ 4,cbufferAllocater->BindAndAttachData(5,&BRIGHTNESS,sizeof(float)) });

	graphicsDevice.DrawMesh("2DPlane");
	graphicsDevice.ExecuteRenderQueue();
	graphicsDevice.ExecuteCommands();

	// ここからブラー

	for (int i = 0; i < 6; ++i)
	{
		graphicsDevice.SetShaderResourceDescriptorHeap();
		graphicsDevice.SetLayer("BloomLayer_" + std::to_string(i));
		graphicsDevice.SetShader("GaussBlurShader");

		renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
		renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
		renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2, &material, sizeof(GE::Material)) });
		renderQueue->AddSetConstantBufferInfo({ 4,cbufferAllocater->BindAndAttachData(5,&gaussFilterData[i],sizeof(GE::Math::GaussFilterData)) });

		if (i == 0)
		{
			renderQueue->AddSetShaderResource({ 5,graphicsDevice.GetLayerManager()->Get("demoLayer")->GetRenderTexture()->GetSRVNumber() });
		}
		else
		{
			renderQueue->AddSetShaderResource({ 5,graphicsDevice.GetLayerManager()->Get("BloomLayer_" + std::to_string(i - 1))->GetRenderTexture()->GetSRVNumber() });
		}

		graphicsDevice.DrawMesh("2DPlane");
		graphicsDevice.ExecuteRenderQueue();
		graphicsDevice.ExecuteCommands();
	}

	// ブラーした結果を元画像に合成
	graphicsDevice.SetShaderResourceDescriptorHeap();
	graphicsDevice.SetDefaultRenderTarget();
	graphicsDevice.SetShader("MixedTextureShader");


	renderQueue->AddSetConstantBufferInfo({ 0,cbufferAllocater->BindAndAttachData(0, &modelMatrix, sizeof(GE::Math::Matrix4x4)) });
	renderQueue->AddSetConstantBufferInfo({ 1,cbufferAllocater->BindAndAttachData(1, &cameraInfo, sizeof(GE::CameraInfo)) });
	renderQueue->AddSetConstantBufferInfo({ 2,cbufferAllocater->BindAndAttachData(2, &material, sizeof(GE::Material)) });
	renderQueue->AddSetShaderResource({ 3,graphicsDevice.GetLayerManager()->Get("resultLayer")->GetRenderTexture()->GetSRVNumber() });
	renderQueue->AddSetShaderResource({ 4,graphicsDevice.GetLayerManager()->Get("EffectLayer")->GetRenderTexture()->GetSRVNumber() });
	renderQueue->AddSetShaderResource({ 5,graphicsDevice.GetLayerManager()->Get("BloomLayer_" + std::to_string(1))->GetRenderTexture()->GetSRVNumber() });
	renderQueue->AddSetShaderResource({ 6,graphicsDevice.GetLayerManager()->Get("BloomLayer_" + std::to_string(3))->GetRenderTexture()->GetSRVNumber() });
	renderQueue->AddSetShaderResource({ 7,graphicsDevice.GetLayerManager()->Get("BloomLayer_" + std::to_string(5))->GetRenderTexture()->GetSRVNumber() });
	graphicsDevice.DrawMesh("2DPlane");

	graphicsDevice.ExecuteRenderQueue();

	GE::GUIManager::EndFrame();

	graphicsDevice.ExecuteCommands();
	graphicsDevice.ScreenFlip();
	return true;
}
