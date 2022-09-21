#pragma once
#include <GatesEngine/Header/GameFramework/Component/Component.h>
#include <string>

class SpriteParticleComponent : public GE::Component
{
private:
	std::string textureName = "NotSetTexture";	//�摜��
	int textureNum = 1;							//�A�ԉ摜����
	float texChangeSpeed = 0.1f;				//�摜�؂�ւ��Ԋu
	float lifeTime = 1.0f;						//�\������

	GE::Math::Vector3 initPosition = {};		//�����ʒu
	GE::Math::Vector3 velocity = {};			//�ړ���
	GE::Math::Vector3 accel = {};				//������

	bool isStart = false;						//�`��E�Đ��J�n������
	bool isLoopAnime = false;					//�A�j���[�V���������[�v�����邩

	float moveTimer = 0;						//�ړ��p�^�C�}�[
	float animeTimer = 0;						//�A�j���؂�ւ��p�^�C�}�[

	bool isStartDelay;
	float startDelay;
	float delayTimer;

public:
	void Start() override;
	void Update(float deltaTime) override;
	void LateDraw() override;

	/// <summary>
	/// �������@�S�Ă̏�񂪐������̏�ԂɂȂ�܂�
	/// </summary>
	void Initialize();

	/// <summary>
	/// �Đ��J�n
	/// </summary>
	void StartAnime(bool isDelay = false,float delayTime = 0);

	/// <summary>
	/// �^�C�}�[�X�V
	/// </summary>
	/// <param name="deltaTime"></param>
	void UpdateTimer(float deltaTime);

	/// <summary>
	/// �ʒu�X�V
	/// </summary>
	void UpdatePos();

	void UpdateDelayTimer(float deltaTime);

	/// <summary>
	/// �A�j���[�V�����Ɏg�p����e�N�X�`���̖��O�w��
	/// �A�Ԃ�����ꍇ�� �Z�Z�Z_0�̂悤�ȃt�H�[�}�b�g�ɂ��A���O�w��ɂ�"_0"���Ȃ��B
	/// </summary>
	/// <param name="textureName">�摜��</param>
	void SetTextureName(const std::string& textureName) { this->textureName = textureName; }

	/// <summary>
	/// �A�ԉ摜�̖����w��
	/// </summary>
	/// <param name="textureNum">�A�ԉ摜�̖���</param>
	void SetTextureNum(int textureNum) { this->textureNum = textureNum; }

	/// <summary>
	/// �摜�؂�ւ��X�s�[�h�w��
	/// </summary>
	/// <param name="texChangeSpeed">�摜�؂�ւ��X�s�[�h</param>
	void SetTexChangeSpeed(float texChangeSpeed) { this->texChangeSpeed = texChangeSpeed; }

	/// <summary>
	/// �`�掞�Ԃ��w��
	/// </summary>
	/// <param name="lifeTime">�`�掞��</param>
	void SetLifeTime(float lifeTime) { this->lifeTime = lifeTime; }

	/// <summary>
	/// �ړ��ʎw��
	/// </summary>
	/// <param name="velocity">�ړ���</param>
	void SetVelocity(const GE::Math::Vector3& velocity) { this->velocity = velocity; }

	/// <summary>
	/// �����ʎw��
	/// </summary>
	/// <param name="accel">������</param>
	void SetAccel(const GE::Math::Vector3& accel) { this->accel = accel; }

	/// <summary>
	/// �A�j���[�V���������[�v�����邩�w��
	/// </summary>
	/// <param name="isLoopAnime">�A�j���[�V���������[�v�����邩</param>
	void SetIsLoopAnime(bool isLoopAnime) { this->isLoopAnime = isLoopAnime; }

	/// <summary>
	/// �摜�̏����ʒu�w��
	/// </summary>
	/// <param name="position">�摜�̏����ʒu</param>
	void SetInitPosition(const GE::Math::Vector3& initPosition) { this->initPosition = initPosition; }

	/// <summary>
	/// �摜�̑傫���w�� (�s�N�Z���P��)
	/// </summary>
	/// <param name="scale">�摜�̑傫��</param>
	void SetScale(const GE::Math::Vector3& scale) { transform->scale = scale; }
};

