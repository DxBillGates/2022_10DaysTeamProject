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
	static const GE::Math::Vector3 SPRITE_SIZE;				//�����X�P�[��
	static const int MAX_GENERATE_COUNT = 12;				//�ő�G������ (�ő僉�C�t)
	static const float MIN_SCALE;							//�ŏ��X�P�[��
	static const float MOVE_SPEED;							//��������

private:
	
	GE::GameObjectManager* pGameObjectManager = nullptr;	//�ʏ�G�l�~�[�����p
	GE::InputDevice* inputDevice;							//�f�o�b�O�p
	std::vector<NormalEnemyComponent*> normalEnemies;		//NormalEnemy�̊Ǘ��R���e�i
	int life = MAX_GENERATE_COUNT;							//���C�t
	float velocity = 0;										//���ړ��p -1�ō��A+1�ŉE�A0�Œ�~
	bool isGenerate = false;								//�G�����t���O
	float scaleDownMag = 1.0f;								//�k���{��
	float scaleDownTimer = 0;								//�X�P�[���k���^�C�}�[
	MoveEntity* pPlayerMoveEntity = nullptr;				//�v���C���[MoveEntity�|�C���^
	GE::Math::Vector3* pPlayerPos = nullptr;				//�v���C���[�ʒu�|�C���^

	// 1�t���[�����Ƀv���C���[�ɍU�����ꂽ��
	bool isHitPlayer;
	bool beforeIsHitPlayer;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnCollision(GE::GameObject* other) override;
	void OnGui() override;

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

