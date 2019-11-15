#pragma once
#include "EventBus.h"

class Updatable
{
public:
	template<typename T> static void Register(...) {}
	template<typename T, typename = decltype(&T::Update)>
	static void Register()
	{
		ECS::EventBus<Updatable>::Register<T>(&T::Update);
	}

	static void Update(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Updatable>::Post(ctx, registry);
	}
};

class Renderable
{
public:
	template<typename T> static void Register(...) {}
	template<typename T, typename = decltype(&T::RenderInitialize), typename = decltype(&T::Render), typename = decltype(&T::RenderFinalize)>
	static void Register()
	{
		ECS::EventBus<Renderable, 0>::Register<T>(&T::RenderInitialize);
		ECS::EventBus<Renderable>::Register<T>(&T::Render);
		ECS::EventBus<Renderable, 1>::Register<T>(&T::RenderFinalize);
	}

	static void RenderInitialize(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Renderable, 0>::Post(ctx, registry);
	}

	static void Render(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Renderable>::Post(ctx, registry);
	}

	static void RenderFinalize(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Renderable, 1>::Post(ctx, registry);
	}
};