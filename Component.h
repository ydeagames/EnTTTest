#pragma once
#include "Serialize.h"

namespace ECS
{
	template<typename Registry, typename Base>
	class Dependency
	{
	private:
		Registry& registry;

	public:
		Dependency(Registry& registry)
			: registry(registry)
		{
		}

	private:
		template<typename T>
		class Creation
		{
		public:
			void on(Registry& registry, typename Registry::entity_type entity)
			{
				registry.get_or_assign<T>(entity);
			}
		};

		template<typename T>
		class Deletion
		{
		public:
			void on(Registry& registry, typename Registry::entity_type entity)
			{
				registry.reset<T>(entity);
			}
		};

		template<typename Dependency>
		void DependsOns()
		{
			Creation<Dependency> cre;
			registry.construction<Base>().connect<Creation<Dependency>, & Creation<Dependency>::on>(&cre);
			Deletion<Base> del;
			registry.destruction<Dependency>().connect<Deletion<Base>, & Deletion<Base>::on>(&del);
		}

	public:
		template<typename... Dependency>
		void DependsOn()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (DependsOns<Dependency>(), 0)... };
			(void)accumulator;
		}
	};

	template<typename Registry>
	class ComponentDependency
	{
	private:
		template<typename Component, typename = decltype(&Component::Dependency<Component>)>
		static void DependsOn0(int, Registry & reg)
		{
			Dependency<Registry, Component> dep(reg);
			Component::Dependency(dep);
		}

		template<typename Component>
		static void DependsOn0(bool, Registry& reg)
		{
		}

	public:
		template<typename Component>
		static void DependsOn(Registry& reg)
		{
			DependsOn0<Component>(0, reg);
		}
	};

	template<typename Registry>
	class ComponentGui
	{
	private:
		template<typename Component, typename = decltype(&Component::EditorGui)>
		static void EditorWidget0(int, GameContext & ctx, Registry & reg, MM::ImGuiEntityEditor<Registry> & editor)
		{
			auto* ctxptr = &ctx;
			editor.registerComponentWidgetFn(
				reg.type<Component>(),
				[ctxptr](auto& reg, auto entity) {
					GameObject o{ &reg, entity };
					reg.get<Component>(entity).EditorGui(*ctxptr, o);
				});
		}

		template<typename Component>
		static void EditorWidget0(bool, GameContext& ctx, Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
		}

	public:
		template<typename Component>
		static void EditorWidget(GameContext& ctx, Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			EditorWidget0<Component>(0, ctx, reg, editor);
		}
	};

	template<typename Registry>
	class ComponentLifecycle
	{
	private:
		template<typename Component, typename = decltype(&Component::Awake)>
		static void Awake0(int, GameContext & ctx, Registry & registry)
		{
			class Creation
			{
			public:
				GameContext& ctx;

			public:
				void on(Registry& registry, typename Registry::entity_type entity)
				{
					registry.get<Component>(entity).Awake(ctx);
				}
			};
			Creation cre;
			registry.construction<Component>().connect<Creation, & Creation::on>(&cre);
		}

		template<typename Component>
		static void Awake0(bool, GameContext& ctx, Registry& reg)
		{
		}

		template<typename Component, typename = decltype(&Component::Destroy)>
		static void Destroy0(int, GameContext & ctx, Registry & registry)
		{
			class Deletion
			{
			public:
				GameContext& ctx;

			public:
				void on(Registry& registry, typename Registry::entity_type entity)
				{
					registry.get<Component>(entity).Destroy(ctx);
				}
			};
			Deletion del(ctx);
			registry.destruction<Component>().connect<Deletion, & Deletion::on>(&del);
		}

		template<typename Component>
		static void Destroy0(bool, GameContext& ctx, Registry& reg)
		{
		}

	public:
		template<typename Component>
		static void Lifecycle(GameContext& ctx, Registry& reg)
		{
			Awake0<Component>(0, ctx, reg);
			Destroy0<Component>(0, ctx, reg);
		}
	};

	// Declaration of a template
	template<typename Registry, typename Components, typename Tags, typename Events>
	class ComponentManager;

	template<typename Registry, typename... Components, typename... Tags, typename... Events>
	class ComponentManager<Registry, std::tuple<Components...>, std::tuple<Tags...>, std::tuple<Events...>>
	{
	private:
		template<typename Event>
		static void InitializeEvent()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (Event::Register<Components>(), 0)... };
			(void)accumulator;
		}

		template<typename Registry, typename Component>
		static void InitializeEditorComponent(GameContext& ctx, Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			editor.registerTrivial<Component>(reg, ECS::IdentifierResolver::name<Component>());
			ComponentGui<Registry>::EditorWidget<Component>(ctx, reg, editor);
		}

		template<typename Registry, typename Component>
		static void InitializeDependency(Registry& reg)
		{
			ComponentDependency<Registry>::DependsOn<Component>(reg);
		}

		template<typename Registry, typename Component>
		static void InitializeLifecycleEvent(Registry& reg)
		{
			ComponentDependency<Registry>::DependsOn<Component>(reg);
		}

	public:
		static void InitializeEvents()
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEvent<Events>(), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeEditorComponents(GameContext& ctx, Registry& reg, MM::ImGuiEntityEditor<Registry>& editor)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeEditorComponent<Registry, Components>(ctx, reg, editor), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeDependency(Registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeDependency<Registry, Components>(reg), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void InitializeLifecycleEvents(Registry& reg)
		{
			using accumulator_type = int[];
			accumulator_type accumulator = { 0, (InitializeLifecycleEvent<Registry, Components>(reg), 0)... };
			(void)accumulator;
		}

		template<typename Registry>
		static void CloneComponents(Registry& reg, const std::vector<typename Registry::entity_type>& srcs, const std::vector<typename Registry::entity_type>& dsts)
		{
			ComponentClone<Registry>::Clone(reg, srcs, dsts);
		}

		template<typename Registry>
		static bool LoadScene(const std::string& location, Registry& scene)
		{
			std::ifstream storage(location);
			return ObjectSerializer<Registry, std::tuple<Components...>, std::tuple<Tags...>>::Import(storage, scene.restore());
		}

		template<typename Registry>
		static bool SaveScene(const std::string& location, const Registry& scene)
		{
			std::ofstream storage(location);
			return ObjectSerializer<Registry, std::tuple<Components...>, std::tuple<Tags...>>::Export(storage, scene.snapshot());
		}

		template<typename Registry>
		static bool LoadEntity(const std::string& location, Registry& scene, typename Registry::entity_type entity)
		{
			std::ifstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONInputArchive archive{ storage };
					EntityImporter<Registry> earchive(archive);
					{
						archive.setNextName("components");
						archive.startNode();
						earchive.component<Components...>(scene, entity);
						archive.finishNode();
					}
					return true;
				}
				catch (cereal::Exception e)
				{
					// —áŠO
				}
			}
			return false;
		}

		template<typename Registry>
		static bool SaveEntity(const std::string& location, const Registry& scene, typename Registry::entity_type entity)
		{
			std::ofstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONOutputArchive archive{ storage };
					EntityExporter<Registry> earchive(archive);
					{
						archive.setNextName("components");
						archive.startNode();
						earchive.component<Components...>(scene, entity);
						archive.finishNode();
					}
					return true;
				}
				catch (cereal::Exception)
				{
					// —áŠO
				}
			}
			return false;
		}
	};
}