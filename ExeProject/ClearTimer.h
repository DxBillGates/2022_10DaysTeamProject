#pragma once

class ClearTimer
{
private:
	static float timer;
	static bool isStartTimer;

public:
	static void Initialize();
	static void Update(float deltaTime);
	static void Start();
	static void Stop();
	static void OnGui();

	static float GetClearTime() { return timer; }
};

