#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	using Components = ComponentManager<
		Transform,
		MoveUpdater,
		MoveDownUpdater,
		PrimitiveRenderer,
		UpdateRenderer
	>;
	Components::InitializeEvents<
		Updatable,
		Renderable
	>();

	//if (Components::LoadScene("scene.json", m_scene))
	//{
	//}
	//else
	{
		{
			auto obj1 = m_scene.create();
			m_scene.assign<Transform>(obj1, Transform());
			//m_scene.assign<PrimitiveRenderer>(obj1, PrimitiveRenderer());
			m_scene.assign<UpdateRenderer>(obj1, UpdateRenderer());
			//m_scene.assign<entt::tag<"enemy"_hs>>(obj1);
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

		Components::SaveScene("scene.json", m_scene);
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
