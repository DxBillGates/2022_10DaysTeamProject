#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>

enum class EnemyState {
	Generating,
	Flying,
	Falling,
	Walking
};

class NormalEnemyComponent : public GE::Component
{
private:
	//状態
	EnemyState enemyState = EnemyState::Generating;

	//生成時の移動前座標
	GE::Math::Vector3 generateBeforePos;
	//生成時の移動後座標
	GE::Math::Vector3 generateAfterPos;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

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
	void SetGeneratePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);
};

