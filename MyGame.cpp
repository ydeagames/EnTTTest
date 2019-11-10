#include "pch.h"
#include "MyGame.h"
#include "Components.h"

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	std::ifstream istorage("scene.json");
	if (istorage)
	{
		cereal::JSONInputArchive input{ istorage };
		m_scene.restore().entities(input).destroyed(input)
			.component<Transform, PrimitiveRenderer>(input);
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
			//m_scene.assign<std::unique_ptr<Updater>>(obj1, std::make_unique<MoveUpdater>());
			//m_scene.assign<std::unique_ptr<Updater>>(obj1, std::make_unique<MoveDownUpdater>());
		}

		{
			std::ofstream ostorage("scene.json");
			if (ostorage)
			{
				// output finishes flushing its contents when it goes out of scope
				cereal::JSONOutputArchive output{ ostorage };
				m_scene.snapshot().entities(output).destroyed(output)
					.component<Transform, PrimitiveRenderer>(output);
			}
		}
	}
}

void MyGame::Update()
{
	m_scene.view<std::unique_ptr<Updater>>().each([&](auto& entity, std::unique_ptr<Updater>& renderer) {
		renderer->Update(*m_context, m_scene, entity);
		});
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
