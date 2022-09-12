#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>

class Effect
{
protected:
	GE::FlagController isActive;
public:
	Effect();
	virtual ~Effect();

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Draw(GE::IGraphicsDeviceDx12* graphicsDevice);

	// �G�t�F�N�g���Đ��J�n
	virtual void Active();

	bool IsActive();
};
