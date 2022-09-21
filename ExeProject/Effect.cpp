#include "Effect.h"

Effect::Effect()
	: isActive()
	, originPosition()
	, originScale(1)
{
}

Effect::~Effect()
{
}

void Effect::Initialize()
{
	isActive.Initialize();

	const float EFFECT_TIME = 1;
	isActive.SetMaxTimeProperty(EFFECT_TIME);

	originPosition = GE::Math::Vector3();
}

void Effect::Update(float deltaTime)
{
	if (isActive.GetFlag() == false)return;

	if (isActive.GetOverTimeTrigger() == true) 
	{
		isActive.SetTime(1);
		isActive.SetFlag(false);
	}

	isActive.Update(deltaTime);
}

void Effect::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
}

void Effect::Active(const GE::Math::Vector3& position, float scale)
{
	isActive.Initialize();
	isActive.SetFlag(true);
	originScale = scale;

	originPosition = position;
}

bool Effect::IsActive()
{
	return isActive.GetFlag();
}
