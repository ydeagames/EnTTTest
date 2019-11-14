#pragma once

class GameContext;

template<typename Events, uint64_t... meta>
class EventBus
{
private:
	using Func = void(GameContext& ctx, entt::DefaultRegistry& registry);
	static std::vector<std::function<Func>>& handlers()
	{
		static std::vector<std::function<Func>> value;
		return value;
	}

	template<typename T, typename F>
	static int RegisterOnce(F f)
	{
		handlers().push_back([f](GameContext& ctx, entt::DefaultRegistry& registry) {
			registry.view<T>().each([f, &ctx, &registry](auto& entity, T& comp) {
				(comp.*f)(ctx, registry, entity);
				});
			});
		return 0;
	}

public:
	static void Post(GameContext& ctx, entt::DefaultRegistry& registry)
	{
		for (auto& func : handlers())
			func(ctx, registry);
	}

public:
	template<typename T, typename F>
	static void Register(F f)
	{
		static int once = RegisterOnce<T>(f);
	}
};

class Updatable
{
public:
	template<typename T> static void Register(...) {}
	template<typename T, typename = decltype(&T::Update)>
	static void Register()
	{
		EventBus<Updatable>::Register<T>(&T::Update);
	}

	static void Update(GameContext& ctx, entt::DefaultRegistry& registry)
	{
		EventBus<Updatable>::Post(ctx, registry);
	}
};

class Renderable
{
public:
	template<typename T> static void Register(...) {}
	template<typename T, typename = decltype(&T::RenderInitialize), typename = decltype(&T::Render), typename = decltype(&T::RenderFinalize)>
	static void Register()
	{
		EventBus<Renderable, 0>::Register<T>(&T::RenderInitialize);
		EventBus<Renderable>::Register<T>(&T::Render);
		EventBus<Renderable, 1>::Register<T>(&T::RenderFinalize);
	}

	static void RenderInitialize(GameContext& ctx, entt::DefaultRegistry& registry)
	{
		EventBus<Renderable, 0>::Post(ctx, registry);
	}

	static void Render(GameContext& ctx, entt::DefaultRegistry& registry)
	{
		EventBus<Renderable>::Post(ctx, registry);
	}

	static void RenderFinalize(GameContext& ctx, entt::DefaultRegistry& registry)
	{
		EventBus<Renderable, 1>::Post(ctx, registry);
	}
};