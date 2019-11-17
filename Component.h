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
		struct Creation
		{
			void on(Registry& registry, typename Registry::entity_type entity)
			{
				registry.get_or_assign<T>(entity);
			}
		};

		template<typename T>
		struct Deletion
		{
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

	// Declaration of a template
	template<typename Components, typename Events, typename Tags>
	class ComponentManager;

	template<typename... Components, typename... Events, typename... Tags>
	class ComponentManager<std::tuple<Components...>, std::tuple<Events...>, std::tuple<Tags...>>
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

		static bool LoadScene(const std::string& location, entt::registry& scene)
		{
			std::ifstream storage(location);
			if (storage)
			{
				try
				{
					cereal::JSONInputArchive archive{ storage };
					ObjectSnapshotLoader<entt::entity> oarchive(archive);
					ObjectTagSnapshotLoader<entt::entity> tarchive(archive);
					ObjectComponentSnapshotLoader<entt::entity> carchive(archive);
					auto snap = scene.restore();
					{
						{
							archive.setNextName("entities");
							archive.startNode();
							{
								archive.setNextName("created");
								archive.startNode();
								snap.entities(oarchive);
								archive.finishNode();
							}
							{
								archive.setNextName("destroyed");
								archive.startNode();
								snap.destroyed(oarchive);
								archive.finishNode();
							}
							archive.finishNode();
						}
						{
							archive.setNextName("tags");
							archive.startNode();
							tarchive.tag<Components...>(snap);
							archive.finishNode();
						}
						{
							archive.setNextName("components");
							archive.startNode();
							carchive.component<Components...>(snap);
							archive.finishNode();
						}
						//snap.orphans();
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

		static bool SaveScene(const std::string& location, const entt::registry& scene)
		{
			std::ofstream storage(location);
			if (storage)
			{
				try
				{
					// output finishes flushing its contents when it goes out of scope
					cereal::JSONOutputArchive archive{ storage };
					ObjectSnapshot<entt::entity> oarchive(archive);
					ObjectTagSnapshot<entt::entity> tarchive(archive);
					ObjectComponentSnapshot<entt::entity> carchive(archive);
					auto snap = scene.snapshot();
					{
						{
							archive.setNextName("entities");
							archive.startNode();
							{
								archive.setNextName("created");
								archive.startNode();
								archive.makeArray();
								snap.entities(oarchive);
								archive.finishNode();
							}
							{
								archive.setNextName("destroyed");
								archive.startNode();
								archive.makeArray();
								snap.destroyed(oarchive);
								archive.finishNode();
							}
							archive.finishNode();
						}
						{
							archive.setNextName("tags");
							archive.startNode();
							tarchive.tag<Tags...>(snap);
							archive.finishNode();
						}
						{
							archive.setNextName("components");
							archive.startNode();
							carchive.component<Components...>(snap);
							archive.finishNode();
						}
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