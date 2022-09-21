#include "SpriteParticleManager.h"

GE::GameObjectManager* SpriteParticleManager::pGameObjectManager;
std::vector<SpriteParticleComponent*> SpriteParticleManager::particles;

int SpriteParticleManager::accessIndex = 0;

void SpriteParticleManager::StaticInitialize()
{
    //最大個数のゲームオブジェクトをあらかじめ生成
    for (int i = 0; i < MAX_PARTICLE_COUNT; i++) {
		//particle生成
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

	//インデックスを増やす
	accessIndex++;
	if (accessIndex >= MAX_PARTICLE_COUNT) {
		accessIndex = 0;
	}

	return res;
}
