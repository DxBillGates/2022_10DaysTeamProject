#pragma once

#include "Effect.h"

class DotExplosionEffect : public Effect
{
private:
	const float EFFECT_TIME = 0.5f;

	GE::Math::Vector3 size;
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw(GE::IGraphicsDeviceDx12* graphicsDevice) override;

	void Active(const GE::Math::Vector3& position, float scale = 1) override;
};
