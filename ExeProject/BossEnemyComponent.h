#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include <vector>

//デバッグ用
#include <GatesEngine/Header/Input/InputDevice.h>

class BossEnemyComponent : public GE::Component
{
private:
	//通常エネミー生成用
	GE::GameObjectManager* pGameObjectManager = nullptr;

	//デバッグ用
	GE::InputDevice* inputDevice;

	//NormalEnemyの管理コンテナ
	std::vector<GE::GameObject*> normalEnemies;

	//横移動用
	float velocity = 0;
	float moveSpeed = 0.0f;

	bool isGenerate = false;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// 横移動させる
	/// </summary>
	void Move();

	/// <summary>
	/// 通常エネミー生成
	/// </summary>
	void GenerateNormalEnemy();

	/// <summary>
	/// ゲームオブジェクトマネージャセット
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { this->pGameObjectManager = pGameObjectManager; }
};

