#include "MonitorEffect.h"

std::unordered_map<std::string, MonitorTimer> MonitorEffect::monitorEffects;
const float MonitorEffect::MAX_TIME = 0.25f;
const int MonitorEffect::FLUSH_NUM = 4;

void MonitorEffect::Initialize()
{
    for (auto& v : monitorEffects) {
        v.second.isStart = false;
        v.second.timer = MAX_TIME;
    }
}

void MonitorEffect::Update(float deltaTime)
{
    for (auto& v : monitorEffects) {
        if (v.second.isStart) {
            v.second.timer += deltaTime;
            if (v.second.timer > MAX_TIME) {
                v.second.timer = MAX_TIME;
            }
        }
    }
}

bool MonitorEffect::IsDraw(const std::string& key)
{
    return (int)(monitorEffects[key].timer / MAX_TIME * FLUSH_NUM * 2) % 2 == 0;
}

void MonitorEffect::StartEffect(const std::string& key)
{
    monitorEffects[key].isStart = true;
    monitorEffects[key].timer = 0;
}

void MonitorEffect::Add(const std::string& key)
{
    monitorEffects.emplace(key, MonitorTimer{ false, MAX_TIME });
}
