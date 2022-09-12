#include "ClearTimer.h"
#include <GatesEngine/Header/GUI/GUIManager.h>

float ClearTimer::timer = 0;
bool ClearTimer::isStartTimer = false;

void ClearTimer::Initialize()
{
	timer = 0;
}

void ClearTimer::Start()
{
	isStartTimer = true;
}

void ClearTimer::Update(float deltaTime)
{
	if (isStartTimer) { timer += deltaTime; }
}

void ClearTimer::Stop()
{
	isStartTimer = false;
}

void ClearTimer::OnGui()
{
	ImGui::Begin("ClearTime");
	ImGui::Text("ClearTime:%f", timer);
	ImGui::End();
}
