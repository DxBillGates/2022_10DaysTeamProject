#pragma once
#include "MoveEntity.h"

#include <GatesEngine/Header/GameFramework/Component/Component.h>

class ShadowPlayerComponent : public GE::Component
{
private:
	MoveEntity moveEntity;
	bool autoMove;

	// テクスチャアニメーション時に何番までアニメーションさせるか
	static const int MAX_ANIMATION_NUMBER;
	// 何秒でアニメーションを切り替えるか
	static const float ADD_DRAW_ANIMATION_NUMBER_TIME;

	static const GE::Math::Vector2 TEXTURE_SIZE;
	static const GE::Math::Vector2 CLIP_SIZE;

	float drawAnimationTimer;
	int drawAnimationNumber;
public:
	ShadowPlayerComponent();

	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnGui() override;

	MoveEntity* GetMoveEntity();

	bool ChackMovable();
	void UpdateAttackable();
};
