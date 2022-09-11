#include "CollisionManager.h"
#include <GatesEngine/Header/GameFramework/Collision/CollisionManager.h>

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Update(float deltaTime)
{
	for (auto& enemy : enemies)
	{
		if (GE::CollisionManager::CheckHit(player.collider, enemy.collider))
		{
			//player.object->OnCollision(enemy.object);
			enemy.object->OnCollision(player.object);
		}

		if (GE::CollisionManager::CheckHit(shadowPlayer.collider, enemy.collider))
		{
			shadowPlayer.object->OnCollision(enemy.object);
			enemy.object->OnCollision(shadowPlayer.object);
		}
	}

	//// デバッグ用
	//if (GE::CollisionManager::CheckHit(player.collider, shadowPlayer.collider))
	//{
	//	player.object->OnCollision(shadowPlayer.object);
	//	shadowPlayer.object->OnCollision(player.object);
	//}
}

void CollisionManager::SetPlayer(GE::GameObject* pPlayer, GE::ICollider* collider)
{
	player.object = pPlayer;
	player.collider = collider;
}

void CollisionManager::SetShadowPlayer(GE::GameObject* pPlayer, GE::ICollider* collider)
{
	shadowPlayer.object = pPlayer;
	shadowPlayer.collider = collider;
}

void CollisionManager::AddEnemy(GE::GameObject* enemy, GE::ICollider* collider)
{
	enemies.push_back({ enemy,collider });
}

CollisionManager::CollisionManager()
	: player()
	, shadowPlayer()
{
}
