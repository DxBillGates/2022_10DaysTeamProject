#include "SpriteParticleManager.h"

GE::GameObjectManager* SpriteParticleManager::pGameObjectManager;
std::vector<SpriteParticleComponent*> SpriteParticleManager::particles;

int SpriteParticleManager::accessIndex = 0;

void SpriteParticleManager::StaticInitialize()
{
    //�ő���̃Q�[���I�u�W�F�N�g�����炩���ߐ���
    for (int i = 0; i < MAX_PARTICLE_COUNT; i++) {
		//particle����
		auto* particle = pGameObjectManager->AddGameObject(new GE::GameObject());
		particle->SetName("particle_" + std::to_string(i));
		particle->GetTransform()->position = { 0,0,0 };
		particle->SetDrawAxisEnabled(true);
		auto* spriteParticleComponent = particle->AddComponent<SpriteParticleComponent>();
		spriteParticleComponent->Initialize();
		particle->Awake();
		particle->Start();

		particles.push_back(spriteParticleComponent);
    }

	accessIndex = 0;
}

void SpriteParticleManager::AllInit()
{
	for (int i = 0; i < MAX_PARTICLE_COUNT; i++) {
		particles[i]->Initialize();
	}
}

SpriteParticleComponent* SpriteParticleManager::AddParticle()
{
	SpriteParticleComponent* res = particles[accessIndex];

	//�C���f�b�N�X�𑝂₷
	accessIndex++;
	if (accessIndex >= MAX_PARTICLE_COUNT) {
		accessIndex = 0;
	}

	return res;
}
