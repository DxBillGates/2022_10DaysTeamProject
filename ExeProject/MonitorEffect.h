#pragma once
#include <unordered_map>
#include <string>

struct MonitorTimer {
	bool isStart = false;
	float timer = 0;
};

class MonitorEffect
{
private:
	static const float MAX_TIME;	//点滅している時間
	static const int FLUSH_NUM;		//点滅する回数

	static std::unordered_map<std::string, MonitorTimer> monitorEffects;	//コンテナ

public:
	static void Initialize();

	static void Update(float deltaTime);

	static bool IsDraw(const std::string& key);

	static bool IsStart(const std::string& key) { return monitorEffects[key].isStart; }

	static void StartEffect(const std::string& key);

	static void Add(const std::string& key);
};

