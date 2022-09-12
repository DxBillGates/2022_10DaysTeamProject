#pragma once
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>

enum class GameState {
	GAME,
	RESULT,
};

class GameUtility
{
private:
	static float timer;
	static bool isStartTimer;
	static GameState gameState;

	static GE::IGraphicsDeviceDx12* graphicsDevice;

private:
	static void Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name);

public:
	static void Initialize();
	static void UpdateTimer(float deltaTime);
	static void TimerStart();
	static void TimerStop();
	static void OnGui();

	static void DrawBackground();

	static float GetClearTime() { return timer; }
	static const GameState& GetGameState() { return gameState; }

	static void SetGameState(const GameState& state) { gameState = state; }

	static void SetGraphicsDevice(GE::IGraphicsDeviceDx12* gDevice) { GameUtility::graphicsDevice = gDevice; }
};
