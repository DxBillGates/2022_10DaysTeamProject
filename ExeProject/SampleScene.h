#pragma once
#include "PlayerComponent.h"
#include "BossEnemyComponent.h"
#include <GatesEngine/Header/GameFramework/Scene/Scene.h>
#include <GatesEngine/Header/GameFramework/Collision/ICollider.h>

class SampleScene : public GE::Scene
{
private:
	GE::ICollider* col1, * col2;
	PlayerComponent* playerComponent;
	BossEnemyComponent* bossEnemyComponent;

	GE::GameObjectManager particleManager;

	bool isSkipTutorial = false;

	int cursol = 0;

	// 左スティックが前フレームに一定値以上だったか
	bool isLStickMovable;
	bool isBeforeLStickMovable;
public:
	SampleScene();
	SampleScene(const std::string& sceneName);
	~SampleScene();
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void LateDraw() override;

	void UpdateCursol();
};
