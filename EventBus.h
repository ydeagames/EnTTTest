#pragma once
#include "Scene.h"
#include "GameObject.h"

class GameContext;

namespace ECS
{
	template<typename Events, uint64_t meta = 0, typename... Args>
	class EventBus
	{
	private:
		template<typename T>
		struct started_t {};

		using Func = void(Scene& registry, Args&&... args);
		static std::vector<std::function<Func>>& handlers()
		{
			static std::vector<std::function<Func>> value;
			return value;
		}

		template<typename T, typename F>
		static void RegisterCustomOnce(F f)
		{
			handlers().push_back([f](Scene& registry, auto&& ... args) {
				registry.registry.view<T>().each([f, &registry, &args](auto& entity, T& comp) {
					GameObject o{ &registry.registry, entity };
					f(o, comp, std::forward<Args>(args)...);
					});
				});
		}

		template<typename T, typename... F>
		static void RegisterOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& o, T& comp, auto&& ... args) {
				using accumulator_type = int[];
				accumulator_type accumulator = { 0, ((comp.*f)(o, args...), 0)... };
				(void)accumulator;
			});
		}

		template<typename T, typename... F>
		static void RegisterFirstOnce(F&& ... f)
		{
			RegisterCustomOnce<T>([f...](auto& o, T& comp, auto&& ... args) {
				if (!o.registry->has<started_t<T>>(o.entity))
				{
					struct Listener
					{
						void on(entt::registry& registry, entt::entity entity)
						{
							registry.reset<started_t<T>>(entity);
						}
					} listener;
					entt::registry& registry = *o.registry;
					registry.destruction<T>().connect<Listener, & Listener::on>(&listener);
					registry.assign<started_t<T>>(o.entity);
					using accumulator_type = int[];
					accumulator_type accumulator = { 0, ((comp.*f)(o, args...), 0)... };
					(void)accumulator;
				}
			});
		}

	public:
		static void Post(Scene& registry, Args&& ... args)
		{
			for (auto& func : handlers())
				func(registry, std::forward<Args>(args)...);
		}

	public:
		template<typename T, typename F>
		static void Register(F f)
		{
			static int once = (RegisterOnce<T>(f), 0);
		}

		template<typename T, typename F>
		static void RegisterCustom(F f)
		{
			static int once = (RegisterCustomOnce<T>(f), 0);
		}

		template<typename T, typename F>
		static void RegisterFirst(F f)
		{
			static int once = (RegisterFirstOnce<T>(f), 0);
		}
	};
}