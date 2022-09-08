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
	void Active(float time);
private:
	HitStopManager();
	HitStopManager(const HitStopManager&) = delete;
	~HitStopManager() = default;
	HitStopManager& operator=(const HitStopManager&) = delete;
};
