#include "SampleScene.h"
#include "PlayerComponent.h"
#include "ShadowPlayerComponent.h"
#include "PlayerAttackManager.h"
#include "HitStopManager.h"
#include <GatesEngine/Header\GameFramework\Component\SampleComponent.h>
#include <GatesEngine/Header\GameFramework\Component\SphereCollider.h>
#include <GatesEngine/Header\GameFramework\Component\BoxCollider.h>
#include <GatesEngine/Header\GameFramework\Collision\CollisionManager.h>

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

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("test1");
		testObject->GetTransform()->position = { 1050,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent < GE::SphereCollider >();
		auto* sampleComponent = testObject->AddComponent<PlayerComponent>();
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		col1 = sampleCollider;

		playerAttackManager->SetPlayer(testObject,sampleComponent->GetMoveEntity());
	}

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("test2");
		testObject->GetTransform()->position = { 1300,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent<GE::BoxCollider>();
		auto* sampleComponent = testObject->AddComponent<ShadowPlayerComponent>();
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 1 });
		sampleCollider->SetType(GE::ColliderType::OBB);
		col2 = sampleCollider;

		playerAttackManager->SetShadowPlayer(testObject, sampleComponent->GetMoveEntity());
	}

	{
		auto* testObject = gameObjectManager.AddGameObject(new GE::GameObject());
		testObject->SetName("test3");
		testObject->GetTransform()->position = { 0,0,0 };
		testObject->SetDrawAxisEnabled(true);
		auto* sampleCollider = testObject->AddComponent<GE::BoxCollider>();
		bossEnemyComponent = testObject->AddComponent<BossEnemyComponent>();
		bossEnemyComponent->SetPGameObjectManager(&gameObjectManager);
		sampleCollider->SetCenter({ 0,0,0 });
		sampleCollider->SetSize({ 2 });
		sampleCollider->SetType(GE::ColliderType::OBB);
	}
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
	gameObjectManager.Awake();
	gameObjectManager.Start();

	PlayerAttackManager::GetInstance()->Initialize();
	HitStopManager::GetInstance()->Initialize();
}

void SampleScene::Update(float deltaTime)
{
	bossEnemyComponent->GenerateNormalEnemy();
	gameObjectManager.Update(deltaTime);

	PlayerAttackManager::GetInstance()->Update(deltaTime);
	HitStopManager::GetInstance()->Update(deltaTime);

	if (GE::CollisionManager::CheckHit(col1, col2))
	{
		col1->Hit(col2, nullptr);
		col2->Hit(col1, nullptr);
	}
}

void SampleScene::Draw()
{
	gameObjectManager.Draw();
}

void SampleScene::LateDraw()
{
	gameObjectManager.LateDraw();
}
