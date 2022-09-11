#pragma once
#include "MoveEntity.h"
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/Input/InputDevice.h>

class PlayerComponent : public GE::Component
{
private:
	GE::InputDevice* inputDevice;
	MoveEntity moveEntity;


	// 無敵時間を管理
	GE::FlagController invincibleFlag;

	// ノックバック関連
	GE::FlagController knockbackFlag;
	GE::Math::Vector3 setKnockbackVector;
	GE::Math::Vector3 knockbackVelocity;
public:
	PlayerComponent();

	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	//void OnGui() override;
	void OnCollision(GE::GameObject* other)override;

	MoveEntity* GetMoveEntity();
	void Knockback(const GE::Math::Vector3& otherPosition);
	void UpdateKnockback(float deltaTime);

	bool CheckMovable();
	void UpdateAttackable();
};
