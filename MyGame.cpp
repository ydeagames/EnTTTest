#include "pch.h"
#include "MyGame.h"
#include "Components.h"

class ComponentNameResolver
{
private:
	struct general_ {};
	struct special_ : general_ {};

	template<typename T, typename = std::enable_if<!std::is_member_pointer<decltype(&T::ComponentName)>::value>::type>
	static const char* name0(special_)
	{
		return T::ComponentName;
	}

	template<typename T>
	static const char* name0(general_)
	{
		const char* name = typeid(T).name();
		return std::strchr(name, ' ') + 1;
	}

public:
	template<typename T>
	static const char* name()
	{
		return name0<T>(special_());
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

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	std::ifstream istorage("scene.json");
	if (istorage)
	{
		cereal::JSONInputArchive input{ istorage };
		ObjectSnapshotLoader<decltype(m_scene)::entity_type> oinput(input);
		ObjectComponentSnapshotLoader<decltype(m_scene)::entity_type> cinput(input);
		auto loader = m_scene.restore();
		{
			{
				input.setNextName("entities");
				input.startNode();
				{
					input.setNextName("created");
					input.startNode();
					loader.entities(oinput);
					input.finishNode();
				}
				{
					input.setNextName("destroyed");
					input.startNode();
					loader.destroyed(oinput);
					input.finishNode();
				}
				input.finishNode();
			}
			{
				input.setNextName("components");
				input.startNode();
				cinput.component<
					Transform,
					MoveUpdater,
					MoveDownUpdater,
					PrimitiveRenderer,
					UpdateRenderer
				>(loader);
				input.finishNode();
			}
		}
	}
	else
	{
		{
			auto obj1 = m_scene.create();
			m_scene.assign<Transform>(obj1, Transform());
			//m_scene.assign<PrimitiveRenderer>(obj1, PrimitiveRenderer());
			m_scene.assign<UpdateRenderer>(obj1, UpdateRenderer());
		}
		{
			auto obj1 = m_scene.create();
			auto transform = Transform();
			transform.position.x = 1;
			m_scene.assign<Transform>(obj1, std::move(transform));
			m_scene.assign<PrimitiveRenderer>(obj1, PrimitiveRenderer());
		}
		{
			auto obj1 = m_scene.create();
			auto transform = Transform();
			transform.position.x = -1;
			m_scene.assign<Transform>(obj1, std::move(transform));
			m_scene.assign<PrimitiveRenderer>(obj1, PrimitiveRenderer());
			m_scene.assign<MoveUpdater>(obj1, MoveUpdater());
			m_scene.assign<MoveDownUpdater>(obj1, MoveDownUpdater());
		}

		{
			std::ofstream ostorage("scene.json");
			if (ostorage)
			{
				// output finishes flushing its contents when it goes out of scope
				cereal::JSONOutputArchive output{ ostorage };
				ObjectSnapshot<decltype(m_scene)::entity_type> ooutput(output);
				ObjectComponentSnapshot<decltype(m_scene)::entity_type> coutput(output);
				auto saver = m_scene.snapshot();
				{
					{
						output.setNextName("entities");
						output.startNode();
						{
							output.setNextName("created");
							output.startNode();
							output.makeArray();
							saver.entities(ooutput);
							output.finishNode();
						}
						{
							output.setNextName("destroyed");
							output.startNode();
							output.makeArray();
							saver.destroyed(ooutput);
							output.finishNode();
						}
						output.finishNode();
					}
					{
						output.setNextName("components");
						output.startNode();
						coutput.component<
							Transform,
							MoveUpdater,
							MoveDownUpdater,
							PrimitiveRenderer,
							UpdateRenderer
						>(saver);
						output.finishNode();
					}
				}
			}
		}
	}
}

void MyGame::Update()
{
	Updatable::Update(*m_context, m_scene);
}

void MyGame::RenderInitialize()
{
	Renderable::RenderInitialize(*m_context, m_scene);
}

void MyGame::Render()
{
	Renderable::Render(*m_context, m_scene);
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(*m_context, m_scene);
}

MyGame::~MyGame()
{
}
