#pragma once
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include "SpriteParticleComponent.h"
#include <vector>

class SpriteParticleManager
{
private:
	static const int MAX_PARTICLE_COUNT = 32;
	
private:
	static GE::GameObjectManager* pGameObjectManager;	//ゲームオブジェクトマネージャポインタ
	static std::vector<SpriteParticleComponent*> particles;		//パーティクル配列

	static int accessIndex;								//生成時に返すポインタ用インデックス

public:
	/// <summary>
	/// 制的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 全てのパーティクル情報を初期化
	/// </summary>
	static void AllInit();

	/// <summary>
	/// パーティクル生成 (MAX_PARTICLE_COUNT 個目以降は0番目のものを書き換えるので注意)
	/// この関数でコンポーネントのポインタ取得し、そのポインタ経由で情報を書き換えること
	/// </summary>
	static SpriteParticleComponent* AddParticle();

	/// <summary>
	/// ゲームオブジェクトマネージャセット
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	static void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { SpriteParticleManager::pGameObjectManager = pGameObjectManager; }

};

