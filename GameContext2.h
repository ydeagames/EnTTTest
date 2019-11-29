#pragma once



#include "ServiceLocator.h"



class GameContext2 final
{
	public:
		template<typename Context, typename ConcreteContext>
		static void Register(ConcreteContext* context)
		{
			ServiceLocator<Context>::Register(context);
		}


		template<typename Context, typename ConcreteContext>
		static void Register(std::unique_ptr<ConcreteContext>& context)
		{
			ServiceLocator<Context>::Register(context);
		}


		template<typename Context, typename ConcreteContext>
		static void Register(std::unique_ptr<ConcreteContext>&& context)
		{
			ServiceLocator<Context>::Register(std::move(context));
		}
		

		template<typename Context>
		static Context* Get()
		{
			return ServiceLocator<Context>::Get();
		}


		template<typename Context>
		static void Reset()
		{
			ServiceLocator<Context>::Reset();
		}
};


class GameContext3 final
{
private:
	class GameContextImpl
	{
	private:
		entt::registry registry;
		entt::entity entity = registry.create();

	public:
		template<typename Component, typename... Args>
		Component& Register(Args&& ... args)
		{
			return registry.assign<Component>(entity, std::forward<Args>(args)...);
		}

		template<typename Component>
		bool Has() const
		{
			return registry.has<Component>(entity);
		}

		template<typename Component>
		void Remove()
		{
			return registry.remove<Component>(entity);
		}

		template<typename Component>
		inline Component& Get()
		{
			return registry.get<Component>(entity);
		}
	};

	class Impl
	{
	public:
		GameContextImpl context;

	public:
		static Impl* s_impl;

		Impl()
		{
			assert(!s_impl && "GameContext is a singleton");
			s_impl = this;
		}

		~Impl()
		{
			s_impl = nullptr;
		}
	};

	std::unique_ptr<Impl> pImpl;

public:
	GameContext3() noexcept(false)
		: pImpl(std::make_unique<Impl>())
	{
	}

public:
	template<typename Component, typename... Args>
	static Component& Register(Args&& ... args)
	{
		return Impl::s_impl->context.Register<Component, Args...>(std::forward<Args>(args)...);
	}

	template<typename Component>
	static bool Has()
	{
		return Impl::s_impl->context.Has<Component>();
	}

	template<typename Component>
	static void Remove()
	{
		Impl::s_impl->context.Remove<Component>();
	}

	template<typename Component>
	static inline Component& Get()
	{
		return Impl::s_impl->context.Get<Component>();
	}
};