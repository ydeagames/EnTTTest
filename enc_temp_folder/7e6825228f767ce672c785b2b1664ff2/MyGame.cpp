#include "pch.h"
#include "MyGame.h"
#include "Components.h"

template <typename Entity>
class ObjectSnapshotLoader
{
private:
	cereal::JSONInputArchive& archive;

public:
	ObjectSnapshotLoader(cereal::JSONInputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity& entity)
	{
		archive(entity);
	}

	template<typename T>
	void operator()(Entity& entity, T& component)
	{
		archive(entity, component);
	}
};

template <typename Entity>
class ObjectComponentSnapshotLoader
{
private:
	cereal::JSONInputArchive& archive;

public:
	ObjectComponentSnapshotLoader(cereal::JSONInputArchive& archive)
		: archive(archive)
	{
	}

public:
	void operator()(Entity& entity)
	{
		archive(entity);
	}

	template<typename T>
	void operator()(Entity& entity, T& component)
	{
		archive(entity, component);
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
			archive(std::to_string(entity));
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
		assert(stack >= 0 && "stack is corrupted");
		if (stack == 0)
		{
			stack = entity;
			archive.startNode();
		}
		else
		{
			stack--;
			archive(entity);
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
	}

	template<typename T>
	void operator()(Entity entity, const T& component)
	{
		{
			stack--;
			archive(cereal::make_nvp(std::to_string(entity), component));
		}
		if (stack == 0)
		{
			archive.finishNode();
		}
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
		input.setNextName("entities");
		input.startNode();
		loader.entities(oinput);
		input.finishNode();
		input.setNextName("destroyed");
		input.startNode();
		loader.destroyed(oinput);
		input.finishNode();
		input.setNextName("components");
		input.startNode();
		loader.component<
			Transform,
			PrimitiveRenderer,
			UpdateRenderer
		>(cinput);
		input.finishNode();
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
				output.setNextName("entities");
				output.startNode();
				output.makeArray();
				saver.entities(ooutput);
				output.finishNode();
				output.setNextName("destroyed");
				output.startNode();
				output.makeArray();
				saver.destroyed(ooutput);
				output.finishNode();
				output.setNextName("components");
				output.startNode();
				output.makeArray();
				saver.component<
					Transform,
					PrimitiveRenderer,
					UpdateRenderer
				>(coutput);
				output.finishNode();
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
