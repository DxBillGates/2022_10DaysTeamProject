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
	//初期スケール
	static const GE::Math::Vector3 SPRITE_SIZE;

	//最大敵生成回数 (最大ライフ)
	static const int MAX_GENERATE_COUNT = 10;

	//最小スケール
	static const float MIN_SCALE;

	//動く速さ
	static const float MOVE_SPEED;

private:
	//通常エネミー生成用
	GE::GameObjectManager* pGameObjectManager = nullptr;

	//デバッグ用
	GE::InputDevice* inputDevice;

	//NormalEnemyの管理コンテナ
	std::vector<NormalEnemyComponent*> normalEnemies;

	//ライフ
	int life = MAX_GENERATE_COUNT;

	//横移動用
	float velocity = 0;	//-1で左、+1で右、0で停止

	//敵生成フラグ
	bool isGenerate = false;

	//縮小倍率
	float scaleDownMag = 1.0f;

	//スケール縮小タイマー
	float scaleDownTimer = 0;

	//プレイヤーMoveEntityポインタ
	MoveEntity* pPlayerMoveEntity = nullptr;

	//プレイヤー位置ポインタ
	GE::Math::Vector3* pPlayerPos = nullptr;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

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

