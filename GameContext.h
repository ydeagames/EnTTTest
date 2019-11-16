#pragma once
#include "DeviceResources.h"

class GameContext final
{
public:
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;

public:
	DX::DeviceResources* dr = nullptr;

public:
	entt::registry registry;
	entt::entity entity;

public:
	GameContext()
		: registry()
		, entity(registry.create())
	{
	}

public:
	template<typename Component, typename... Args>
	Component& Register(Args&& ... args)
	{
		return registry.assign<Component>(entity, std::forward<Component>(args)...);
	}

	template<typename Component>
	const Component& Get() const
	{
		return registry.get<T>(entity);
	}

	template<typename Component>
	inline Component& Get()
	{
		return registry.get<Component>(entity);
	}
};