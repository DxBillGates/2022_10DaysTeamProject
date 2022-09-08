#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include "MoveEntity.h"

//�f�o�b�O�p
#include <GatesEngine/Header/Input/InputDevice.h>

//�G�̏��
enum class EnemyState {
	GENERATING,	//������
	FLYING,		//�󒆕��V��
	FALLING,	//������
	WALKING,	//���s(�v���C���[�Ɍ������Ēǔ�)��
	DEADING,	//���S���o��
	DEAD		//���S (�폜�҂�)
};

class NormalEnemyComponent : public GE::Component
{
//�萔
private:
	static const float MOVE_SPEED;

private:

	//�f�o�b�O�p
	GE::InputDevice* inputDevice = nullptr;

	//���
	EnemyState enemyState = EnemyState::GENERATING;

	//�������̈ړ��O���W
	GE::Math::Vector3 moveBeforePos = {};
	//�������̈ړ�����W
	GE::Math::Vector3 moveAfterPos = {};

	//�ړ��p�^�C�}�[
	float moveTimer = 0;

	//�󒆕��V�p
	float loopTimer = 0;

	//�p��
	StanceState stanceState = StanceState::NORMAL;

	//�{�X�̈ʒu�|�C���^
	GE::Math::Vector3* pBossPosition = nullptr;

	//�v���C���[�ʒu�擾�pMoveEntity�|�C���^
	MoveEntity* pPlayerMoveEntity = nullptr;

	//�v���C���[�ʒu�|�C���^
	GE::Math::Vector3* pPlayerPos = nullptr;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// �摜�̌��������������`�F�b�N
	/// </summary>
	void CheckStance();

	/// <summary>
	/// �������̍X�V����
	/// </summary>
	void UpdateGenerating();

	/// <summary>
	/// ���V���̍X�V����
	/// </summary>
	void UpdateFlying();

	/// <summary>
	/// �������̍X�V����
	/// </summary>
	void UpdateFalling();

	/// <summary>
	/// ���s���̍X�V����
	/// </summary>
	void UpdateWalking();

	/// <summary>
	/// ���S���̍X�V����
	/// </summary>
	void UpdateDeading();

	/// <summary>
	/// ���g������ł��邩
	/// </summary>
	/// <returns>���S��Ԃ�</returns>
	bool IsDead()const { return enemyState == EnemyState::DEAD; };

	/// <summary>
	/// ��������ۂ̈ړ��O�ƈړ���̍��W�Z�b�g
	/// </summary>
	/// <param name="before">�ړ��O���W</param>
	/// <param name="after">�ړ�����W</param>
	void SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);

	/// <summary>
	/// �{�X�̈ʒu�|�C���^�Z�b�g
	/// </summary>
	/// <param name="pBossPosition">�{�X�̈ʒu�|�C���^</param>
	void SetPBossPosition(GE::Math::Vector3* pBossPosition) { this->pBossPosition = pBossPosition; };

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
};

