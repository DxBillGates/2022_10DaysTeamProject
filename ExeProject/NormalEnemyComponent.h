#pragma once
#include "MoveEntity.h"
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <GatesEngine/Header/Audio/AudioManager.h>

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

enum EnemyAnimationState
{
	DAMAGED = 3,
};

////�p��
//enum class StanceState
//{
//	NORMAL,		//�ʏ� (������)
//	INVERSE,	//�㉺���]
//};
class NormalEnemyComponent : public GE::Component
{
//�萔
private:
	static const float INIT_SCALE;							//�����X�P�[��
	static const float WALK_SPEED;							//���̈ړ�����Ői�ދ���
	static const float MAX_FLYING_LOOP_TIMER;				//�󒆕��V�^�C�}�[�ő�l
	static const float MAX_WALK_LOOP_TIMER;					//���s�p�^�C�}�[�ő�l
	static const float MAX_FLYING_ANIME_LOOP_TIMER;			//�󒆕��V�A�j���^�C�}�[�ő�l
	static const float FLYING_ANIME_LOOP_TIMER_SPEED;		//�󒆕��V�A�j���^�C�}�[�X�V�{��
	static const float MAX_WALKING_ANIME_LOOP_TIMER;		//���s�A�j���^�C�}�[�ő�l
	static const float WALKING_ANIME_LOOP_TIMER_SPEED;		//���s�A�j���^�C�}�[�X�V�{��

private:

	GE::InputDevice* inputDevice = nullptr;					//�f�o�b�O�p
	GE::AudioManager* pAudioManager = nullptr;				//�I�[�f�B�I�Đ��p
	EnemyState enemyState = EnemyState::GENERATING;			//���
	GE::Math::Vector3 moveBeforePos = {};					//�������̈ړ��O���W
	GE::Math::Vector3 moveAfterPos = {};					//�������̈ړ�����W
	float moveTimer = 0;									//�ړ��p�^�C�}�[
	float flyingLoopTimer = 0;								//�󒆕��V�p�^�C�}�[
	float walkingLoopTimer = 0;								//���s�p�^�C�}�[
	float flyingAnimeLoopTimer = 0;							//�󒆕��V�A�j���^�C�}�[
	float walkingAnimeLoopTimer = 0;						//���s�A�j���^�C�}�[
	StanceState stanceState = StanceState::NORMAL;			//�p��
	GE::Math::Vector3* pBossPosition = nullptr;				//�{�X�̈ʒu�|�C���^
	MoveEntity* pPlayerMoveEntity = nullptr;				//�v���C���[�ʒu�擾�pMoveEntity�|�C���^
	GE::Math::Vector3* pPlayerPos = nullptr;				//�v���C���[�ʒu�|�C���^

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;
	void OnCollision(GE::GameObject* other)override;

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

	/// <summary>
	/// �I�[�f�B�I�}�l�[�W���Z�b�g
	/// </summary>
	/// <param name=""></param>
	void SetPAudioManager(GE::AudioManager* pAudioManager) { this->pAudioManager = pAudioManager; }
};

