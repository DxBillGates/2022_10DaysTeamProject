#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include <vector>
#include "NormalEnemyComponent.h"
#include "MoveEntity.h"

//�f�o�b�O�p
#include <GatesEngine/Header/Input/InputDevice.h>

class BossEnemyComponent : public GE::Component
{
	//�萔
private:
	//�����X�P�[��
	static const GE::Math::Vector3 SPRITE_SIZE;

	//�ő�G������ (�ő僉�C�t)
	static const int MAX_GENERATE_COUNT = 10;

	//�ŏ��X�P�[��
	static const float MIN_SCALE;

	//��������
	static const float MOVE_SPEED;

private:
	//�ʏ�G�l�~�[�����p
	GE::GameObjectManager* pGameObjectManager = nullptr;

	//�f�o�b�O�p
	GE::InputDevice* inputDevice;

	//NormalEnemy�̊Ǘ��R���e�i
	std::vector<NormalEnemyComponent*> normalEnemies;

	//���C�t
	int life = MAX_GENERATE_COUNT;

	//���ړ��p
	float velocity = 0;	//-1�ō��A+1�ŉE�A0�Œ�~

	//�G�����t���O
	bool isGenerate = false;

	//�k���{��
	float scaleDownMag = 1.0f;

	//�X�P�[���k���^�C�}�[
	float scaleDownTimer = 0;

	//�v���C���[MoveEntity�|�C���^
	MoveEntity* pPlayerMoveEntity = nullptr;

	//�v���C���[�ʒu�|�C���^
	GE::Math::Vector3* pPlayerPos = nullptr;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// ���ړ�������
	/// </summary>
	void Move();

	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// ���g�̑傫���X�V
	/// </summary>
	void UpdateScale();

	/// <summary>
	/// ���C�t�X�V
	/// </summary>
	void UpdateLife();

	/// <summary>
	/// �ʏ�G�l�~�[����
	/// </summary>
	void GenerateNormalEnemy();

	/// <summary>
	/// �Q�[���I�u�W�F�N�g�}�l�[�W���Z�b�g
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { this->pGameObjectManager = pGameObjectManager; }

	/// <summary>
	/// �v���C���[��MoveEntity�|�C���^�Z�b�g
	/// </summary>
	/// <param name="pMoveEntity">�v���C���[��MoveEntity�|�C���^</param>
	void SetPPlayerMoveEntity(MoveEntity* pMoveEntity) { pPlayerMoveEntity = pMoveEntity; };

	/// <summary>
	/// �v���C���[�̈ʒu�|�C���^�Z�b�g
	/// </summary>
	/// <param name="pPlayerPos">�v���C���[�̈ʒu�|�C���^</param>
	void SetPPlayerPos(GE::Math::Vector3* pPlayerPos) { this->pPlayerPos = pPlayerPos; }

	/// <summary>
	/// ���g������ł��邩
	/// </summary>
	bool IsDead()const { return life <= 0; }
};

