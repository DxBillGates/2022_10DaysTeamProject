#pragma once
#include <GatesEngine/Header/Graphics/IGraphicsDeviceDx12.h>
#include <array>
#include <future>

class Result
{
private:
	static float timer;
	static bool isStartTimer;
	static std::array<float, 8> ranking;
	static bool isGetRanking;
	static std::thread thread;
	static bool rankingError;
	static int* pCursol;

	static GE::IGraphicsDeviceDx12* graphicsDevice;

private:
	static void Draw(const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, const std::string& name);
	static void DrawNum(const std::string& num, const GE::Math::Vector3& pos, const GE::Math::Vector3& scale, float padding = 0);

public:
	static const float NEXT_TEXT_TIME;

	static void Initialize();
	static void Update(float deltaTime);

	static void Draw();

	static void DrawClear();

	static void DrawGameOver();

	static void EndLoadRanking() { isGetRanking = true; }

	static void StartThread();

	static void JoinThread();

	static bool IsEnableMoveCursol();

	static void SendScore(float time);

	static void GetRanking();

	static void SetPCursol(int* pCursol) { Result::pCursol = pCursol; }

	static void SetGraphicsDevice(GE::IGraphicsDeviceDx12* gDevice) { Result::graphicsDevice = gDevice; }
};