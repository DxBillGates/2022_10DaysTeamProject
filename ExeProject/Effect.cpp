#include "Effect.h"

Effect::Effect()
	: isActive()
	, originPosition()
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
		isActive.SetTime(isActive.GetMaxTimeProperty());
		isActive.SetFlag(false);
	}

	isActive.Update(deltaTime);
}

void Effect::Draw(GE::IGraphicsDeviceDx12* graphicsDevice)
{
}

void Effect::Active(const GE::Math::Vector3& position)
{
	isActive.Initialize();
	isActive.SetFlag(true);

	const float EFFECT_TIME = 1;
	isActive.SetMaxTimeProperty(EFFECT_TIME);

	originPosition = position;
}

bool Effect::IsActive()
{
	return isActive.GetFlag();
}
