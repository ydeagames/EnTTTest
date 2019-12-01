#include "pch.h"
#include "GameObject.h"
#include "Components.h"

entt::entity GameObject::GetParent()
{
	if (registry->has<Transform>())
		return registry->get<Transform>().parent;
	return entt::null;
}

std::vector<entt::entity> GameObject::GetChildren()
{
	std::vector<entt::entity> children;
	registry->view<Transform>().each([&](auto e, Transform& transform) {
		if (transform.parent == entity)
			children.push_back(e);
		});
	return children;
}