#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>

//�f�o�b�O�p
#include <GatesEngine/Header/Input/InputDevice.h>

//�G�̏��
enum class EnemyState {
	GENERATING,	//������
	FLYING,		//�󒆕��V��
	FALLING,	//������
	WALKING		//���s(�v���C���[�Ɍ������Ēǔ�)��
};

enum class StanceState
{
	NORMAL,
	INVERSE,
};

class NormalEnemyComponent : public GE::Component
{
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
	/// �������̈ړ�����
	/// </summary>
	void MoveGenerating();

	/// <summary>
	/// ���V���̈ړ�����
	/// </summary>
	void MoveFlying();

	/// <summary>
	/// �������̈ړ�����
	/// </summary>
	void MoveFalling();

	/// <summary>
	/// ���s���̈ړ�����
	/// </summary>
	void MoveWalking();

	/// <summary>
	/// ��������ۂ̈ړ��O�ƈړ���̍��W�Z�b�g
	/// </summary>
	/// <param name="before">�ړ��O���W</param>
	/// <param name="after">�ړ�����W</param>
	void SetMovePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);
};

