#pragma once
#include "DeviceResources.h"

class Camera
{
public:
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};

class GameContext final
{
private:
	class GameContextImpl
	{
	public:
		template<typename T>
		class Holder
		{
		public:
			virtual ~Holder() = default;
			std::unique_ptr<T> data;
		};
		using ctx_family = entt::Family<struct InternalContextFamily>;
		std::vector<std::unique_ptr<Holder<void>>> pools;

	public:
		template<typename Component, typename... Args>
		Component& Register(Args&& ... args)
		{
			const auto ctype = ctx_family::type<Component>();
			if (!(ctype < pools.size()))
				pools.resize(ctype + 1);
			if (!pools[ctype])
				pools[ctype] = std::make_unique<Holder<Component>>();
			auto& pool = static_cast<Holder<Component>&>(*pools[ctype]);
			pool.data = std::make_unique<Component>(std::forward<Args>(args)...);
			return *pool.data;
		}

		template<typename Component>
		bool Has() const
		{
			const auto ctype = ctx_family::type<Component>();
			return ctype < pools.size() && pools[ctype] && pools[ctype]->data;
		}

		template<typename Component>
		void Remove()
		{
			const auto ctype = ctx_family::type<Component>();
			pools[ctype]->data = nullptr;
		}

		template<typename Component>
		inline Component& Get()
		{
			const auto ctype = ctx_family::type<Component>();
			auto& pool = static_cast<Holder<Component>&>(*pools[ctype]);
			return *pool.data;
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
	GameContext() noexcept(false)
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