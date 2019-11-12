#include "pch.h"
#include "MyGame.h"
#include "Components.h"

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	std::ifstream istorage("scene.json");
	if (!istorage)
	{
		cereal::JSONInputArchive input{ istorage };
		auto loader = m_scene.restore();
		input.setNextName("entities");
		input.startNode();
		loader.entities(input);
		input.finishNode();
		input.setNextName("destroyed");
		input.startNode();
		loader.destroyed(input);
		input.finishNode();
		input.setNextName("components");
		input.startNode();
		loader.component<Transform, PrimitiveRenderer>(input);
		input.finishNode();
	}
	else
	{
		{
			auto obj1 = m_scene.create();
			m_scene.assign<Transform>(obj1, Transform());
			m_scene.assign<PrimitiveRenderer>(obj1, PrimitiveRenderer());
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
				auto saver = m_scene.snapshot();
				output.setNextName("entities");
				output.startNode();
				output.makeArray();
				saver.entities(output);
				output.finishNode();
				output.setNextName("destroyed");
				output.startNode();
				output.makeArray();
				saver.destroyed(output);
				output.finishNode();
				output.setNextName("components");
				output.startNode();
				output.makeArray();
				saver.component<Transform, PrimitiveRenderer>(output);
				output.finishNode();
			}
		}
	}
}

void MyGame::Update()
{
	Updaters::Update(*m_context, m_scene);
}

void MyGame::RenderInitialize()
{
	m_scene.view<PrimitiveRenderer>().each([&](auto& entity, PrimitiveRenderer& renderer) {
		renderer.RenderInitialize(*m_context, m_scene, entity);
		});
}

void MyGame::Render()
{
	m_scene.view<PrimitiveRenderer>().each([&](auto& entity, PrimitiveRenderer& renderer) {
		renderer.Render(*m_context, m_scene, entity);
		});
}

void MyGame::RenderFinalize()
{
	m_scene.view<PrimitiveRenderer>().each([&](auto& entity, PrimitiveRenderer& renderer) {
		renderer.RenderFinalize(*m_context, m_scene, entity);
		});
}

MyGame::~MyGame()
{
}
