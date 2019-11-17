#pragma once
#include "EventBus.h"

class Updatable
{
private:
	template<typename T> static void RegisterFirst(...) {}
	template<typename T, typename = decltype(&T::Start)>
	static void RegisterFirst()
	{
		ECS::EventBus<Updatable, 0>::RegisterFirst<T>(&T::Start);
	}

	template<typename T> static void RegisterTick(...) {}
	template<typename T, typename = decltype(&T::Update)>
	static void RegisterTick()
	{
		ECS::EventBus<Updatable>::Register<T>(&T::Update);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterFirst<T>();
		RegisterTick<T>();
	}

	static void Update(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Updatable, 0>::Post(ctx, registry);
		ECS::EventBus<Updatable>::Post(ctx, registry);
	}
};

class Renderable
{
private:
	template<typename T> static void RegisterFirst(...) {}
	template<typename T, typename = decltype(&T::RenderStart)>
	static void RegisterFirst()
	{
		ECS::EventBus<Renderable, 0>::RegisterFirst<T>(&T::RenderStart);
	}

	template<typename T> static void RegisterTick(...) {}
	template<typename T, typename = decltype(&T::Render)>
	static void RegisterTick()
	{
		ECS::EventBus<Renderable>::Register<T>(&T::Render);
	}

public:
	template<typename T>
	static void Register()
	{
		RegisterFirst<T>();
		RegisterTick<T>();
	}

	static void RenderInitialize(GameContext& ctx, Scene& registry)
	{
		//ECS::EventBus<Renderable, 0>::Post(ctx, registry);
	}

	static void Render(GameContext& ctx, Scene& registry)
	{
		ECS::EventBus<Renderable, 0>::Post(ctx, registry);
		ECS::EventBus<Renderable>::Post(ctx, registry);
	}

	static void RenderFinalize(GameContext& ctx, Scene& registry)
	{
		//ECS::EventBus<Renderable, 1>::Post(ctx, registry);
	}
};