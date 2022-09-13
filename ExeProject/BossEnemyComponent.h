#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include <GatesEngine/Header/Audio/AudioManager.h>
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
	static const float MIN_SCALE;							//最小スケール
	static const float MOVE_SPEED;							//動く速さ

	// テクスチャアニメーション時に何番までアニメーションさせるか
	static const int MAX_ANIMATION_NUMBER;
	static const int DAMAGE_ANIMATION_NUMBER;
	// 何秒でアニメーションを切り替えるか
	static const float CHANGE_ANIMATION_TIME;

	static const GE::Math::Vector2 TEXTURE_SIZE;
	static const GE::Math::Vector2 CLIP_SIZE;

private:
	
	GE::GameObjectManager* pGameObjectManager = nullptr;	//通常エネミー生成用
	GE::AudioManager* pAudioManager = nullptr;				//オーディオ再生用
	GE::InputDevice* inputDevice;							//デバッグ用
	std::vector<NormalEnemyComponent*> normalEnemies;		//NormalEnemyの管理コンテナ
	int maxGenerateCount = 32;				//最大敵生成回数 (最大ライフ)
	int life = maxGenerateCount;							//ライフ
	float velocity = 0;										//横移動用 -1で左、+1で右、0で停止
	bool isGenerate = false;								//敵生成フラグ
	int alreadyGeneratedCount = 0;							//敵を生成した数
	int generateNum = 1;									//一度に生成する敵の数
	float scaleDownMag = 1.0f;								//縮小倍率
	float scaleDownTimer = 0;								//スケール縮小タイマー
	MoveEntity* pPlayerMoveEntity = nullptr;				//プレイヤーMoveEntityポインタ
	GE::Math::Vector3* pPlayerPos = nullptr;				//プレイヤー位置ポインタ

	// 1フレーム中にプレイヤーに攻撃されたか
	bool isHitPlayer;
	bool beforeIsHitPlayer;

	static int generateCountOneAttack;						//1回の攻撃で生成した敵の数　生成ボーナス適用に使用

	float drawAnimationTimer;
	int drawAnimationNumber;

	GE::FlagController damageFlag;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnCollision(GE::GameObject* other) override;
	void OnGui() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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
	/// ダメージ受けたかどうかの管理フラグを更新
	/// </summary>
	void UpdateDamageFlag(float deltaTime);

	/// <summary>
	/// アニメーション関係の更新
	/// </summary>
	void UpdateAnimation(float deltaTime);

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
	/// オーディオマネージャセット
	/// </summary>
	/// <param name=""></param>
	void SetPAudioManager(GE::AudioManager* pAudioManager) { this->pAudioManager = pAudioManager; }

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

	static void ResetGenerateCountOneAttack() { generateCountOneAttack = 0; }
};

