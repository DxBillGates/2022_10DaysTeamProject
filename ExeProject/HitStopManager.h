#pragma once
#include <GatesEngine/Header/GameFramework/FlagController.h>

class HitStopManager
{
private:
	GE::FlagController flag;
public:
	static HitStopManager* GetInstance();

	void Initialize();
	void Update(float deltaTime);

	// ヒットストップをオンにする
	void Active(float time);

	// ヒットストップ中か取得
	bool IsActive();
private:
	HitStopManager();
	HitStopManager(const HitStopManager&) = delete;
	~HitStopManager() = default;
	HitStopManager& operator=(const HitStopManager&) = delete;
};
