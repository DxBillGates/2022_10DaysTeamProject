#pragma once
#include "MoveEntity.h"

#include <GatesEngine/Header/GameFramework/Component/Component.h>

class ShadowPlayerComponent : public GE::Component
{
private:
	MoveEntity moveEntity;
	bool autoMove;
public:
	ShadowPlayerComponent();

	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnGui() override;

	MoveEntity* GetMoveEntity();
};
