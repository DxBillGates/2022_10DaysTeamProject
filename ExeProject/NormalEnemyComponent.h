#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>

//デバッグ用
#include <GatesEngine/Header/Input/InputDevice.h>

//敵の状態
enum class EnemyState {
	GENERATING,	//生成中
	FLYING,		//空中浮遊中
	FALLING,	//落下中
	WALKING,	//歩行(プレイヤーに向かって追尾)中
	DEADING,	//死亡演出中
	DEAD		//死亡 (削除待ち)
};

//姿勢
enum class StanceState
{
	NORMAL,		//通常 (頭が上)
	INVERSE,	//上下反転
};

class NormalEnemyComponent : public GE::Component
{
private:

	//デバッグ用
	GE::InputDevice* inputDevice = nullptr;

	//状態
	EnemyState enemyState = EnemyState::GENERATING;

	//生成時の移動前座標
	GE::Math::Vector3 moveBeforePos = {};
	//生成時の移動後座標
	GE::Math::Vector3 moveAfterPos = {};

	//移動用タイマー
	float moveTimer = 0;

	//空中浮遊用
	float loopTimer = 0;

	//姿勢
	StanceState stanceState = StanceState::NORMAL;

	//ボスの位置ポインタ
	GE::Math::Vector3* pBossPosition = nullptr;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// タイマー更新
	/// </summary>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// 画像の向きが正しいかチェック
	/// </summary>
	void CheckStance();

	/// <summary>
	/// 生成中の更新処理
	/// </summary>
	void UpdateGenerating();

	/// <summary>
	/// 浮遊中の更新処理
	/// </summary>
	void UpdateFlying();

	/// <summary>
	/// 落下中の更新処理
	/// </summary>
	void UpdateFalling();

	/// <summary>
	/// 歩行中の更新処理
	/// </summary>
	void UpdateWalking();

	/// <summary>
	/// 死亡中の更新処理
	/// </summary>
	void UpdateDeading();

	/// <summary>
	/// 生成する際の移動前と移動後の座標セット
	/// </summary>
	/// <param name="before">移動前座標</param>
	/// <param name="after">移動後座標</param>
	void SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);

	/// <summary>
	/// ボスの位置ポインタセット
	/// </summary>
	/// <param name="pBossPosition">ボスの位置ポインタ</param>
	void SetPBossPosition(GE::Math::Vector3* pBossPosition) { this->pBossPosition = pBossPosition; };
};

