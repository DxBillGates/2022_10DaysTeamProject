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
		testObject->SetName("test3");
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
		collisionManager->AddEnemy(testObject, sampleCollider);
	}

	// effect 追加
	{
		auto* effectManager = EffectManager::GetInstance();

		for (int i = 0; i < 3; ++i)
		{
			auto* slashEffect = effectManager->Add<SlashEffect>("slashEffect");
			auto* dotEffect = effectManager->Add<DotExplosionEffect>("dotEffect");
		}
	}

	//パーティクルを最前面に
	SpriteParticleManager::SetPGameObjectManager(&particleManager);
	SpriteParticleManager::StaticInitialize();

	GE::RandomMaker::ResetSeed();
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
	gameObjectManager.Awake();
	gameObjectManager.Start();

	particleManager.Awake();
	particleManager.Start();

	PlayerAttackManager::GetInstance()->Initialize();
	HitStopManager::GetInstance()->Initialize();

	SpriteParticleManager::AllInit();

	EffectManager::GetInstance()->Initialize();
}

void SampleScene::Update(float deltaTime)
{
	bossEnemyComponent->GenerateNormalEnemy();
	gameObjectManager.Update(deltaTime);
	particleManager.Update(deltaTime);

	EffectManager::GetInstance()->Update(deltaTime);
	PlayerAttackManager::GetInstance()->Update(deltaTime);
	HitStopManager::GetInstance()->Update(deltaTime);

	//if (GE::CollisionManager::CheckHit(col1, col2))
	//{
	//	col1->Hit(col2, gameObjectManager.FindGameObject("test2"));
	//	col2->Hit(col1, gameObjectManager.FindGameObject("test1"));
	//}

	CollisionManager::GetInstance()->Update(deltaTime);
}

void SampleScene::Draw()
{
	gameObjectManager.Draw();

	particleManager.Draw();
}

void SampleScene::LateDraw()
{
	gameObjectManager.LateDraw();
	particleManager.LateDraw();
	EffectManager::GetInstance()->Draw(graphicsDevice);
}
