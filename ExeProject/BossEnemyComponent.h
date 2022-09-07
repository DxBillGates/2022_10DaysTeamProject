#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include <vector>

//�f�o�b�O�p
#include <GatesEngine/Header/Input/InputDevice.h>

class BossEnemyComponent : public GE::Component
{
private:
	//�ʏ�G�l�~�[�����p
	GE::GameObjectManager* pGameObjectManager = nullptr;

	//�f�o�b�O�p
	GE::InputDevice* inputDevice;

	//NormalEnemy�̊Ǘ��R���e�i
	std::vector<GE::GameObject*> normalEnemies;

	//���ړ��p
	float velocity = 0;
	float moveSpeed = 0.0f;

	bool isGenerate = false;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// ���ړ�������
	/// </summary>
	void Move();

	/// <summary>
	/// �ʏ�G�l�~�[����
	/// </summary>
	void GenerateNormalEnemy();

	/// <summary>
	/// �Q�[���I�u�W�F�N�g�}�l�[�W���Z�b�g
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { this->pGameObjectManager = pGameObjectManager; }
};

