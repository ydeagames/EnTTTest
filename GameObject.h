#pragma once

class GameObject final
{
public:
	entt::registry* registry;
	entt::entity entity;

public:
	GameObject(entt::registry* registry, entt::entity entity)
		: registry(registry)
		, entity(entity)
	{
	}

	GameObject()
		: registry(nullptr)
		, entity(entt::null)
	{
	}

public:
	template<typename Component>
	bool HasComponent()
	{
		return registry->has<Component>(entity);
	}

	template<typename Component, typename... Args>
	Component AddComponent(Args&&... args)
	{
		return registry->assign<Component>(entity, std::forward<Component>(args)...);
	}

	template<typename Component>
	const Component& GetComponent() const
	{
		return registry->get<T>(entity);
	}

	template<typename Component>
	inline Component& GetComponent()
	{
		return registry->get<Component>(entity);
	}

	entt::entity GetParent();

	std::vector<entt::entity> GetChildren();
};
