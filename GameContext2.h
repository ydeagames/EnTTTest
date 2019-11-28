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
