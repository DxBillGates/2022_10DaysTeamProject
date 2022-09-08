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

	// �q�b�g�X�g�b�v���I���ɂ���
	void Active(float time);

	// �q�b�g�X�g�b�v�����擾
	bool IsActive();
private:
	HitStopManager();
	HitStopManager(const HitStopManager&) = delete;
	~HitStopManager() = default;
	HitStopManager& operator=(const HitStopManager&) = delete;
};
