#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>

class Effect
{
protected:
	GE::FlagController isActive;

	GE::Math::Vector3 originPosition;
public:
	Effect();
	virtual ~Effect();

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Draw(GE::IGraphicsDeviceDx12* graphicsDevice);

	// エフェクトを再生開始
	virtual void Active(const GE::Math::Vector3& position);

	bool IsActive();
};
