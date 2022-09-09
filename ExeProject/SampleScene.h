#pragma once
#include "BossEnemyComponent.h"
#include <GatesEngine/Header/GameFramework/Scene/Scene.h>
#include <GatesEngine/Header/GameFramework/Collision/ICollider.h>

class SampleScene : public GE::Scene
{
private:
	GE::ICollider* col1, * col2;
	BossEnemyComponent* bossEnemyComponent;

	GE::GameObjectManager particleManager;
public:
	SampleScene();
	SampleScene(const std::string& sceneName);
	~SampleScene();
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void LateDraw() override;
};
