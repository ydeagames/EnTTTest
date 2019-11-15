#pragma once

class GameObject final
{
public:
	entt::registry* registry;
	entt::entity entity;

public:
	template<typename Component, typename... Args>
	void AddComponent(Args&&... args)
	{
		registry->assign<Component>(entity, std::forward<Component>(args)...);
	}

	//template<typename Component>
	//const Component& GetComponent() const {
	//{
	//	return registry->get<T>(entity);
	//}

	template<typename Component>
	inline Component& GetComponent()
	{
		return registry->get<Component>(entity);
	}
};

