#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include <vector>
#include "NormalEnemyComponent.h"
#include "MoveEntity.h"

//デバッグ用
#include <GatesEngine/Header/Input/InputDevice.h>

class BossEnemyComponent : public GE::Component
{
//定数
private:
	static const GE::Math::Vector3 SPRITE_SIZE;				//初期スケール
	static const int MAX_GENERATE_COUNT = 12;				//最大敵生成回数 (最大ライフ)
	static const float MIN_SCALE;							//最小スケール
	static const float MOVE_SPEED;							//動く速さ

private:
	
	GE::GameObjectManager* pGameObjectManager = nullptr;	//通常エネミー生成用
	GE::InputDevice* inputDevice;							//デバッグ用
	std::vector<NormalEnemyComponent*> normalEnemies;		//NormalEnemyの管理コンテナ
	int life = MAX_GENERATE_COUNT;							//ライフ
	float velocity = 0;										//横移動用 -1で左、+1で右、0で停止
	bool isGenerate = false;								//敵生成フラグ
	float scaleDownMag = 1.0f;								//縮小倍率
	float scaleDownTimer = 0;								//スケール縮小タイマー
	MoveEntity* pPlayerMoveEntity = nullptr;				//プレイヤーMoveEntityポインタ
	GE::Math::Vector3* pPlayerPos = nullptr;				//プレイヤー位置ポインタ

	// 1フレーム中にプレイヤーに攻撃されたか
	bool isHitPlayer;
	bool beforeIsHitPlayer;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnCollision(GE::GameObject* other) override;
	void OnGui() override;

	/// <summary>
	/// 横移動させる
	/// </summary>
	void Move();

	/// <summary>
	/// タイマー更新
	/// </summary>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// 自身の大きさ更新
	/// </summary>
	void UpdateScale();

	/// <summary>
	/// ライフ更新
	/// </summary>
	void UpdateLife();

	/// <summary>
	/// 通常エネミー生成
	/// </summary>
	void GenerateNormalEnemy();

	/// <summary>
	/// ゲームオブジェクトマネージャセット
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { this->pGameObjectManager = pGameObjectManager; }

	/// <summary>
	/// プレイヤーのMoveEntityポインタセット
	/// </summary>
	/// <param name="pMoveEntity">プレイヤーのMoveEntityポインタ</param>
	void SetPPlayerMoveEntity(MoveEntity* pMoveEntity) { pPlayerMoveEntity = pMoveEntity; };

	/// <summary>
	/// プレイヤーの位置ポインタセット
	/// </summary>
	/// <param name="pPlayerPos">プレイヤーの位置ポインタ</param>
	void SetPPlayerPos(GE::Math::Vector3* pPlayerPos) { this->pPlayerPos = pPlayerPos; }

	/// <summary>
	/// 自身が死んでいるか
	/// </summary>
	bool IsDead()const { return life <= 0; }
};

