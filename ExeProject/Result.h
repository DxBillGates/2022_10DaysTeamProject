#pragma once
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>

class Result
{
private:
	static float timer;
	static bool isStartTimer;

	static GE::IGraphicsDeviceDx12* graphicsDevice;

private:
	static void Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name);
	static void DrawNum(const std::string& num, const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, float padding = 0);

public:
	static void Initialize();
	static void UpdateTimer(float deltaTime);

	static void Draw();

	static void SetGraphicsDevice(GE::IGraphicsDeviceDx12* gDevice) { Result::graphicsDevice = gDevice; }
};

