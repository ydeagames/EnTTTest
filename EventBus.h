#pragma once

class GameContext;

namespace ECS
{
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
}