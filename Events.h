#pragma once
#include "EventBus.h"

class Updatable
{
private:
	template<typename T> static void RegisterStart(...) {}
	template<typename T, typename = decltype(&T::Start)>
	static void RegisterStart()
	{
		ECS::EventBus<Updatable>::RegisterFirst<T>(&T::Start);
	}

	template<typename T> static void RegisterUpdate(...) {}
	template<typename T, typename = decltype(&T::Update)>
	static void RegisterUpdate()
	{
		ECS::EventBus<Updatable>::Register<T>(&T::Update);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterStart<T>();
		RegisterUpdate<T>();
	}

	static void Update(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Updatable>::Post(ctx, registry);
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