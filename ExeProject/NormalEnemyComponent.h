#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>

//デバッグ用
#include <GatesEngine/Header/Input/InputDevice.h>

//敵の状態
enum class EnemyState {
	GENERATING,	//生成中
	FLYING,		//空中浮遊中
	FALLING,	//落下中
	WALKING		//歩行(プレイヤーに向かって追尾)中
};

enum class StanceState
{
	NORMAL,
	INVERSE,
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
	/// 生成中の移動処理
	/// </summary>
	void MoveGenerating();

	/// <summary>
	/// 浮遊中の移動処理
	/// </summary>
	void MoveFlying();

	/// <summary>
	/// 落下中の移動処理
	/// </summary>
	void MoveFalling();

	/// <summary>
	/// 歩行中の移動処理
	/// </summary>
	void MoveWalking();

	/// <summary>
	/// 生成する際の移動前と移動後の座標セット
	/// </summary>
	/// <param name="before">移動前座標</param>
	/// <param name="after">移動後座標</param>
	void SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);
};

