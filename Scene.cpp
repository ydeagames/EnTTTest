#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include "AllComponents.h"

GameObject Scene::Create()
{
	return GameObject{ &registry, registry.create() };
}

Scene::Scene()
{
	Components::InitializeDependency(registry);
}

bool Scene::Load()
{
	bool b = Components::LoadScene(location, registry);
	Components::InitializeDependency(registry);
	return b;
}

bool Scene::Save() const
{
	return Components::SaveScene(location, registry);
}
