#pragma once
#include "MoveEntity.h"
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/Input/InputDevice.h>
#include <GatesEngine/Header/Audio/AudioManager.h>

class PlayerComponent : public GE::Component
{
private:
	// テクスチャアニメーション時に何番までアニメーションさせるか
	static const int MAX_ANIMATION_NUMBER_WALK;
	static const int MAX_ANIMATION_NUMBER_STOP;
	// 何秒でアニメーションを切り替えるか
	static const float CHANGE_ANIMATION_TIME_WALK;
	static const float CHANGE_ANIMATION_TIME_STOP;

	static const GE::Math::Vector2 TEXTURE_SIZE_WALK;
	static const GE::Math::Vector2 TEXTURE_SIZE_STOP;
	static const GE::Math::Vector2 CLIP_SIZE;

	static const float FLASHING_TIME;

	static const int   MAX_HP;
	static const float MOVE_SPEED;

	float drawAnimationTimer;
	int drawAnimationNumber;

	GE::InputDevice* inputDevice;
	MoveEntity moveEntity;


	// 無敵時間を管理
	GE::FlagController invincibleFlag;
	// 無敵時間中点滅用
	float flashingTimer;
	bool isFlashing;

	// ノックバック関連
	GE::FlagController knockbackFlag;
	GE::Math::Vector3 setKnockbackVector;
	GE::Math::Vector3 knockbackVelocity;

	int hp;

	bool isMove;
	bool isDead;

	GE::AudioManager* audioManager;

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

	void DrawHP();

	bool CheckMovable();
	void UpdateAttackable();

	void SetAudioManager(GE::AudioManager* pAudioManager);
private:
	void UpdateInvinsible(float deltaTime);
	void Move(const float GAME_TIME);
};
