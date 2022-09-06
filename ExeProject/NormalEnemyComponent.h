#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>

enum class EnemyState {
	Generating,
	Flying,
	Falling,
	Walking
};

class NormalEnemyComponent : public GE::Component
{
private:
	//���
	EnemyState enemyState = EnemyState::Generating;

	//�������̈ړ��O���W
	GE::Math::Vector3 generateBeforePos;
	//�������̈ړ�����W
	GE::Math::Vector3 generateAfterPos;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

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
	void SetGeneratePos(const GE::Math::Vector3& before, const GE::Math::Vector3& after);
};

