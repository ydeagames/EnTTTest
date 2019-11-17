#pragma once
#include "Scene.h"
#include "GameObject.h"

class GameContext;

namespace ECS
{
	template<typename Events, uint64_t... meta>
	class EventBus
	{
	private:
		template<typename T>
		struct started_t {};

		using Func = void(GameContext& ctx, Scene& registry);
		static std::vector<std::function<Func>>& handlers()
		{
			static std::vector<std::function<Func>> value;
			return value;
		}

		template<typename T, typename F>
		static int RegisterCustomOnce(F f)
		{
			handlers().push_back([f](GameContext& ctx, Scene& registry) {
				registry.registry.view<T>().each([f, &ctx, &registry](auto& entity, T& comp) {
					GameObject o{ &registry.registry, entity };
					f(ctx, o, comp);
					});
				});
			return 0;
		}

		template<typename T, typename... F>
		static int RegisterOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& ctx, auto& o, T& comp) {
				using accumulator_type = int[];
				accumulator_type accumulator = { 0, ((comp.*f)(ctx, o), 0)... };
				(void)accumulator;
			});
			return 0;
		}

		template<typename T, typename... F>
		static int RegisterFirstOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& ctx, auto& o, T& comp) {
				if (!o.registry->has<started_t<T>>(o.entity))
				{
					struct Listener
					{
						void on(entt::registry& registry, entt::entity entity)
						{
							registry.remove<started_t<T>>(entity);
						}
					} listener;
					entt::registry& registry = *o.registry;
					registry.destruction<T>().connect<Listener, & Listener::on>(&listener);
					registry.assign<started_t<T>>(o.entity);
					using accumulator_type = int[];
					accumulator_type accumulator = { 0, ((comp.*f)(ctx, o), 0)... };
					(void)accumulator;
				}
			});
			return 0;
		}

	public:
		static void Post(GameContext& ctx, Scene& registry)
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

		template<typename T, typename F>
		static void RegisterCustom(F f)
		{
			static int once = RegisterCustomOnce<T>(f);
		}

		template<typename T, typename F>
		static void RegisterFirst(F f)
		{
			static int once = RegisterFirstOnce<T>(f);
		}
	};
}