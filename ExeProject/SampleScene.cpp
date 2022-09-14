
#include "SampleScene.h"
#include "PlayerComponent.h"
#include "ShadowPlayerComponent.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"
#include "CollisionManager.h"
#include "SpriteParticleManager.h"
#include "EffectManager.h"
#include "SlashEffect.h"
#include "DotExplosionEffect.h"
#include "BossCrushingEffect.h"
#include "Camera2D.h"
#include "Tutorial.h"
#include "GameUtility.h"
#include "Result.h"
#include "MonitorEffect.h"
#include <GatesEngine/Header\GameFramework\Component\SampleComponent.h>
#include <GatesEngine/Header\GameFramework\Component\SphereCollider.h>
#include <GatesEngine/Header\GameFramework\Component\BoxCollider.h>
#include <GatesEngine/Header\GameFramework\Collision\CollisionManager.h>
#include <GatesEngine/Header/Util/Random.h>

SampleScene::SampleScene()
	: SampleScene("SampleScene")
{
}

SampleScene::SampleScene(const std::string& sceneName)
	: Scene(sceneName)
	, col1(nullptr)
	, col2(nullptr)
{
	auto* playerAttackManager = PlayerAttackManager::GetInstance();
	auto* collisionManager = CollisionManager::GetInstance();
	MoveEntity* pPlayerMoveEntity = nullptr;
	GE::Math::Vector3* pPlayerPos = nullptr;
	GE::Math::Vector3* pShadowPlayerPosition = nullptr;

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("Player");
		testObject->GetTransform()->position = { 1050,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent < GE::SphereCollider >();
		auto* sampleComponent = testObject->AddComponent<PlayerComponent>();
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		col1 = sampleCollider;
		testObject->SetTag("Player");

		pPlayerMoveEntity = sampleComponent->GetMoveEntity();
		pPlayerPos = &testObject->GetTransform()->position;

		playerAttackManager->SetPlayer(testObject, pPlayerMoveEntity);
		collisionManager->SetPlayer(testObject, sampleCollider);
		playerComponent = sampleComponent;
	}

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("ShadowPlayer");
		testObject->GetTransform()->position = { 1300,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent<GE::BoxCollider>();
		auto* sampleComponent = testObject->AddComponent<ShadowPlayerComponent>();
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		sampleCollider->SetType(GE::ColliderType::OBB);
		col2 = sampleCollider;
		testObject->SetTag("ShadowPlayer");

		playerAttackManager->SetShadowPlayer(testObject, sampleComponent->GetMoveEntity());
		collisionManager->SetShadowPlayer(testObject, sampleCollider);

		pShadowPlayerPosition = &testObject->GetTransform()->position;
	}

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("BossEnemy");
		testObject->GetTransform()->position = { 0,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent<GE::BoxCollider>();
		bossEnemyComponent = testObject->AddComponent<BossEnemyComponent>();
		bossEnemyComponent->SetPGameObjectManager(&gameObjectManager);
		bossEnemyComponent->SetPPlayerMoveEntity(pPlayerMoveEntity);
		bossEnemyComponent->SetPPlayerPos(pPlayerPos);
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		sampleCollider->SetType(GE::ColliderType::OBB);

		testObject->SetTag("Boss");
		collisionManager->AddBoss(testObject, sampleCollider);
	}

	// effect 追加
	{
		auto* effectManager = EffectManager::GetInstance();

		for (int i = 0; i < 10; ++i)
		{
			auto* slashEffect = effectManager->Add<SlashEffect>("slashEffect");
			auto* dotEffect = effectManager->Add<DotExplosionEffect>("dotEffect");
			auto* bossCrushingEffect = effectManager->Add<BossCrushingEffect>("bossCrushingEffect");
		}
	}

	//パーティクルを最前面に
	SpriteParticleManager::SetPGameObjectManager(&particleManager);
	SpriteParticleManager::StaticInitialize();

	Result::SetPCursol(&cursol);

	GE::RandomMaker::ResetSeed();
}

SampleScene::~SampleScene()
{
	Result::JoinThread();
}

void SampleScene::Initialize()
{
	Tutorial::SetGraphicsDevice(graphicsDevice);
	GameUtility::SetGraphicsDevice(graphicsDevice);
	Result::SetGraphicsDevice(graphicsDevice);

	MonitorEffect::Initialize();

	Tutorial::Initialize(isSkipTutorial);	//チュートリアルスキップさせるならtrueに

	gameObjectManager.Awake();
	gameObjectManager.Start();

	particleManager.Awake();
	particleManager.Start();

	PlayerAttackManager::GetInstance()->Initialize();
	HitStopManager::GetInstance()->Initialize();

	SpriteParticleManager::AllInit();

	EffectManager::GetInstance()->Initialize();

	Camera2D::GetInstance()->Initialize();
	
	GameUtility::Initialize();

	Result::Initialize();


	playerComponent->SetPAudioManager(audioManager);
	bossEnemyComponent->SetPAudioManager(audioManager);
	PlayerAttackManager::GetInstance()->SetPAudioManager(audioManager);

	cursol = 0;
}

void SampleScene::Update(float deltaTime)
{
	Camera2D::GetInstance()->Update(deltaTime);

	if (Tutorial::IsEndTutorial() && GameUtility::GetClearTime() >= 0.5f) {
		//BGM再生 (いったん毎フレーム実行)
		audioManager->Use("BGM")->Start();
	}

	bossEnemyComponent->GenerateNormalEnemy();
	gameObjectManager.Update(deltaTime);
	particleManager.Update(deltaTime);

	EffectManager::GetInstance()->Update(deltaTime);
	PlayerAttackManager::GetInstance()->Update(deltaTime,playerComponent->IsDead());
	HitStopManager::GetInstance()->Update(deltaTime);
	MonitorEffect::Update(deltaTime);
	//if (GE::CollisionManager::CheckHit(col1, col2))
	//{
	//	col1->Hit(col2, gameObjectManager.FindGameObject("test2"));
	//	col2->Hit(col1, gameObjectManager.FindGameObject("test1"));
	//}
	Tutorial::UpdateTimer(deltaTime);
	GameUtility::UpdateTimer(deltaTime);
	Result::Update(deltaTime);
	
	auto collManager = CollisionManager::GetInstance();
	collManager->Update(deltaTime);

	if ((GameUtility::GetGameState() == GameState::RESULT_CLEAR ||
		GameUtility::GetGameState() == GameState::RESULT_GAMEOVER) &&
		Result::IsEnableMoveCursol()) {
		UpdateCursol();
	}
}

void SampleScene::Draw()
{
	GameUtility::DrawBackground();

	if (GameUtility::GetGameState() == GameState::RESULT_CLEAR ||
		GameUtility::GetGameState() == GameState::RESULT_GAMEOVER) Result::Draw();

	gameObjectManager.Draw();

	Tutorial::Draw();
	GameUtility::OnGui();

	particleManager.Draw();
}

void SampleScene::LateDraw()
{
	gameObjectManager.LateDraw();

	graphicsDevice->ClearLayer("EffectLayer");
	graphicsDevice->SetLayer("EffectLayer");

	particleManager.LateDraw();
	EffectManager::GetInstance()->Draw(graphicsDevice);
}

void SampleScene::UpdateCursol()
{
	isBeforeLStickMovable = isLStickMovable;

	auto ctrler = inputDevice->GetXCtrler();
	const float LSTICK_DEAD_ZONE = 0.15f;
	isLStickMovable = ctrler->GetLStickY() > LSTICK_DEAD_ZONE || ctrler->GetLStickY() < -LSTICK_DEAD_ZONE;

	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::W) ||
		inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::S) ||
		inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::UP) ||
		inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::DOWN) ||
		ctrler->CheckHitButtonTrigger(GE::XInputControllerButton::XINPUT_UP) || 
		ctrler->CheckHitButtonTrigger(GE::XInputControllerButton::XINPUT_DOWN) || 
		(isBeforeLStickMovable == false && isLStickMovable == true))
	{
		//効果音再生
		audioManager->Use("CursolDecide")->Start();

		//カーソルチェンジ (2つしか項目ないのでこれで十分)
		cursol = cursol == 0 ? 1 : 0;
	}

	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::SPACE) ||
		inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::RETURN) ||
		ctrler->CheckHitButton(GE::XInputControllerButton::XINPUT_A)) {
		//効果音再生
		audioManager->Use("CursolDecide")->Start();
	}
	bool isInputExitKey = inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::RETURN);
	if (ctrler->CheckHitButton(GE::XInputControllerButton::XINPUT_A))
	{
		isInputExitKey = true;
	}
	if (inputDevice->GetKeyboard()->CheckPressTrigger(GE::Keys::SPACE))
	{
		isInputExitKey = true;
	}

	if (isInputExitKey == true) {
		//Restart
		if (cursol == 0) {
			gameObjectManager.DeleteGameObjectWithTag("Enemy");
			auto collManager = CollisionManager::GetInstance();
			collManager->EraseEnemy();
			changeSceneInfo.flag = true;
			changeSceneInfo.initNextSceneFlag = true;
			changeSceneInfo.name = this->name;

			//2回目以降はチュートリアルスキップ
			isSkipTutorial = true;
		}
		//Exit
		else {
			//ゲーム終了させるコード追加お願いします
			Scene::isTerminateApplication = true;
		}
	}
}
