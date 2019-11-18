#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Serialize.h"
#include "AllComponents.h"

GameObject Scene::Create()
{
	return GameObject{ &registry, registry.create() };
}

bool Scene::Load()
{
	return Components::LoadScene(location, registry);
}

bool Scene::Save() const
{
	return Components::SaveScene(location, registry);
}
