#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	Components::InitializeEvents();

	if (m_scene.Load("scene.json"))
	{
	}
	else
	{
		{
			auto obj1 = m_scene.Create();
			obj1.AddComponent<Transform>(Transform());
			//obj1.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
			obj1.AddComponent<UpdateRenderer>(UpdateRenderer());
			//obj1.AddTag<entt::tag<"enemy"_hs>>(obj1);
			//obj1.AddTag<int>(4);
			//obj1.AddTag<std::string>(obj1);
		}
		{
			auto obj1 = m_scene.Create();
			auto transform = Transform();
			transform.position.x = 1;
			obj1.AddComponent<Transform>(std::move(transform));
			obj1.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
		}
		{
			auto obj1 = m_scene.Create();
			auto transform = Transform();
			transform.position.x = -1;
			obj1.AddComponent<Transform>(std::move(transform));
			obj1.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
			obj1.AddComponent<MoveUpdater>(MoveUpdater());
			obj1.AddComponent<MoveDownUpdater>(MoveDownUpdater());
		}

		m_scene.Save("scene.json");
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
