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
	return Components::LoadScene(location, registry, [](auto& registry) {
		Components::InitializeDependency(registry);
		Components::InitializeLifecycleEvents(registry);
		});
}

bool Scene::Save() const
{
	return Components::SaveScene(location, registry);
}
