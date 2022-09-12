#pragma once
#include "Effect.h"
#include <GatesEngine/Header/Util/Math/Vector3.h>

class SlashEffect : public Effect
{
private:
	const float EFFECT_TIME = 0.5f;

	float angle;

	GE::Math::Vector3 size;

	GE::Math::Vector3 originPosition;
	GE::Math::Vector3* startPosition;
	GE::Math::Vector3* endPosition;
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw(GE::IGraphicsDeviceDx12* graphicsDevice) override;

	void Active() override;

	void SetStartAndEndPosition(GE::Math::Vector3* start, GE::Math::Vector3* end);
};
