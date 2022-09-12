#include "..\..\..\Header\GameFramework\GameObject\GameObjectManager.h"

GE::GameObjectManager::GameObjectManager()
{
}

GE::GameObjectManager::~GameObjectManager()
{
	for (auto& object : gameObjects)
	{
		delete object;
	}
	gameObjects.clear();
}

void GE::GameObjectManager::Awake()
{
	for (auto& object : gameObjects)
	{
		object->Awake();
	}
}

void GE::GameObjectManager::Start()
{
	for (auto& object : gameObjects)
	{
		object->Start();
	}
}

void GE::GameObjectManager::Update(float deltaTime)
{
	bool isReleaseMode = true;
#ifdef _DEBUG
	isReleaseMode = false;
#endif



	for (auto& object : gameObjects)
	{
		object->Update(deltaTime);

		if (!isReleaseMode)
		{
			// Hierarchy‚ÌGameObject‚ð‘I‘ð‚µ‚½‚çInspector‚É‚»‚ÌGameObject‚ð“o˜^
			if (hierarchyGui.OnGui(object))
			{
				inspectorGui.SetCurrentSelectGameObject(object);
			}
		}
	}

	if (!isReleaseMode)
	{
		inspectorGui.OnGui();
	}
}

void GE::GameObjectManager::Draw()
{
	for (auto& object : gameObjects)
	{
		object->Draw();
	}
}

void GE::GameObjectManager::LateDraw()
{
	for (auto& object : gameObjects)
	{
		object->LateDraw();
	}
}

GE::GameObject* GE::GameObjectManager::AddGameObject(GameObject* newGameObject)
{
	gameObjects.push_back(newGameObject);
	return newGameObject;
}

GE::GameObject* GE::GameObjectManager::FindGameObject(const std::string& name)
{
	GameObject* returnObject = nullptr;
	for (auto& object : gameObjects)
	{
		if (object->GetName() != name)continue;

		returnObject = object;
		break;
	}
	return returnObject;
}

GE::GameObject* GE::GameObjectManager::FindGameObjectWithTag(const std::string& name, const std::string& tag)
{
	GameObject* returnObject = nullptr;
	for (auto& object : gameObjects)
	{
		if (object->GetName() != name)continue;
		if (object->GetTag() != tag)continue;

		returnObject = object;
		break;
	}
	return returnObject;
}

void GE::GameObjectManager::DeleteGameObject(const std::string& name)
{
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetName() == name)
		{
			delete gameObject;
			gameObject = nullptr;
			std::swap(gameObject, gameObjects.back());
			gameObjects.pop_back();
			return;
		}
	}
}

void GE::GameObjectManager::DeleteGameObjectWithTag(const std::string& tag)
{
	int l = gameObjects.size();
	for (int i = 0; i < l; ++i)
	{
		if (gameObjects[i]->GetTag() == tag)
		{
			delete gameObjects[i];
			gameObjects[i] = nullptr;
			std::swap(gameObjects[i], gameObjects.back());
			//gameObjects.erase(gameObjects.begin() + i);
			gameObjects.pop_back();

			l--;
			i--;
		}
	}
}
