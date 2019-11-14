#pragma once

class ComponentNameResolver
{
private:
	template<typename T, typename = decltype(&T::ComponentName)>
	static const char* name0()
	{
		return T::ComponentName;
	}

	template<typename T>
	static const char* name0(...)
	{
		const char* name = typeid(T).name();
		return std::strchr(name, ' ') + 1;
	}

public:
	template<typename T>
	static const char* name()
	{
		return name0<T>();
	}
};

template <typename Entity>
class ObjectSnapshotLoader
{
private:
	cereal::JSONInputArchive& archive;
	Entity stack = 0;

public:
	ObjectSnapshotLoader(cereal::JSONInputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity& entity)
	{
		if (stack == 0)
		{
			cereal::size_type size;
			archive.loadSize(size);
			entity = stack = static_cast<Entity>(size);
		}
		else
		{
			stack--;
			archive(entity);
		}
	}
};

template <typename Entity>
class ObjectComponentSnapshotLoader
{
private:
	cereal::JSONInputArchive& archive;
	Entity stack = 0;

public:
	ObjectComponentSnapshotLoader(cereal::JSONInputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity& entity)
	{
		if (stack == 0)
		{
			archive.startNode();
			cereal::size_type size;
			archive.loadSize(size);
			entity = stack = static_cast<Entity>(size);
		}
		else
		{
			assert(false);
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
	}

	template<typename T>
	void operator()(Entity& entity, T& component)
	{
		if (stack == 0)
		{
			assert(false);
		}
		else
		{
			stack--;
			archive.startNode();
			archive(cereal::make_nvp("id", entity));
			archive(cereal::make_nvp("component", component));
			archive.finishNode();
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
	}

private:
	template<typename Component>
	void component0(entt::SnapshotLoader<Entity>& loader) {
		const char* name = ComponentNameResolver::name<Component>();
		if (archive.hasName(name))
		{
			archive.setNextName(name);
			loader.component<Component>(*this);
		}
	}

public:
	template<typename... Component>
	void component(entt::SnapshotLoader<Entity>& loader) {
		using accumulator_type = int[];
		accumulator_type accumulator = { 0, (component0<Component>(loader), 0)... };
		(void)accumulator;
	}
};

template <typename Entity>
class ObjectSnapshot
{
private:
	cereal::JSONOutputArchive& archive;
	Entity stack = 0;

public:
	ObjectSnapshot(cereal::JSONOutputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity entity)
	{
		if (stack == 0)
		{
			stack = entity;
		}
		else
		{
			stack--;
			archive(entity);
		}
	}
};

template <typename Entity>
class ObjectComponentSnapshot
{
private:
	cereal::JSONOutputArchive& archive;
	Entity stack = 0;

public:
	ObjectComponentSnapshot(cereal::JSONOutputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity entity)
	{
		if (stack == 0)
		{
			archive.startNode();
			archive.makeArray();
			stack = entity;
		}
		else
		{
			assert(false);
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
	}

	template<typename T>
	void operator()(Entity entity, const T& component)
	{
		if (stack == 0)
		{
			assert(false);
		}
		else
		{
			stack--;
			archive.startNode();
			archive(cereal::make_nvp("id", entity));
			archive(cereal::make_nvp("component", component));
			archive.finishNode();
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
	}

private:
	template<typename Component>
	void component0(entt::Snapshot<Entity>& saver) {
		archive.setNextName(ComponentNameResolver::name<Component>());
		saver.component<Component>(*this);
	}

public:
	template<typename... Component>
	void component(entt::Snapshot<Entity>& saver) {
		using accumulator_type = int[];
		accumulator_type accumulator = { 0, (component0<Component>(saver), 0)... };
		(void)accumulator;
	}
};

template<typename... Components>
class ComponentManager
{
private:
	template<typename Event>
	static void InitializeEvent()
	{
		using accumulator_type = int[];
		accumulator_type accumulator = { 0, (Event::Register<Components>(), 0)... };
		(void)accumulator;
	}

public:
	template<typename... Events>
	static void InitializeEvents()
	{
		using accumulator_type = int[];
		accumulator_type accumulator = { 0, (InitializeEvent<Events>(), 0)... };
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
					//{
					//	archive.setNextName("tags");
					//	archive.startNode();
					//	snap.tag<entt::tag<"enemy"_hs>>(carchive);
					//	archive.finishNode();
					//}
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
			catch (cereal::Exception)
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
					//{
					//	archive.setNextName("tags");
					//	archive.startNode();
					//	snap.tag<entt::tag<"enemy"_hs>>(carchive);
					//	archive.finishNode();
					//}
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