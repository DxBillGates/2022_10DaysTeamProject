#pragma once
#include <GatesEngine/Header/GameFramework/GameObject/GameObject.h>

struct CollisionInfo
{
	GE::GameObject* object;
	GE::ICollider* collider;
};

class CollisionManager
{
private:
	CollisionInfo player;
	CollisionInfo shadowPlayer;
	CollisionInfo boss;

	std::vector<CollisionInfo> enemies;
public:
	static CollisionManager* GetInstance();

	void Update(float deltaTime);

	void SetPlayer(GE::GameObject* pPlayer, GE::ICollider* collider);
	void SetShadowPlayer(GE::GameObject* pPlayer, GE::ICollider* collider);
	void AddEnemy(GE::GameObject* enemy, GE::ICollider* collider);
	void AddBoss(GE::GameObject* enemy, GE::ICollider* collider);

	void EraseEnemy();

private:
	CollisionManager();
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;
};
