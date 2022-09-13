#pragma once
#include "MoveEntity.h"

#include <GatesEngine/Header/GameFramework/Component/Component.h>

class ShadowPlayerComponent : public GE::Component
{
private:
	MoveEntity moveEntity;
	bool autoMove;

	// �e�N�X�`���A�j���[�V�������ɉ��Ԃ܂ŃA�j���[�V���������邩
	static const int MAX_ANIMATION_NUMBER;
	// ���b�ŃA�j���[�V������؂�ւ��邩
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
