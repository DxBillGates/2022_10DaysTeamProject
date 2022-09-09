#pragma once
#include <GatesEngine/Header/GameFramework/GameObject/GameObjectManager.h>
#include "SpriteParticleComponent.h"
#include <vector>

class SpriteParticleManager
{
private:
	static const int MAX_PARTICLE_COUNT = 32;
	
private:
	static GE::GameObjectManager* pGameObjectManager;	//�Q�[���I�u�W�F�N�g�}�l�[�W���|�C���^
	static std::vector<SpriteParticleComponent*> particles;		//�p�[�e�B�N���z��

	static int accessIndex;								//�������ɕԂ��|�C���^�p�C���f�b�N�X

public:
	/// <summary>
	/// ���I������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �S�Ẵp�[�e�B�N������������
	/// </summary>
	static void AllInit();

	/// <summary>
	/// �p�[�e�B�N������ (MAX_PARTICLE_COUNT �ڈȍ~��0�Ԗڂ̂��̂�����������̂Œ���)
	/// ���̊֐��ŃR���|�[�l���g�̃|�C���^�擾���A���̃|�C���^�o�R�ŏ������������邱��
	/// </summary>
	static SpriteParticleComponent* AddParticle();

	/// <summary>
	/// �Q�[���I�u�W�F�N�g�}�l�[�W���Z�b�g
	/// </summary>
	/// <param name="pGameObjectManager"></param>
	static void SetPGameObjectManager(GE::GameObjectManager* pGameObjectManager) { SpriteParticleManager::pGameObjectManager = pGameObjectManager; }

};

