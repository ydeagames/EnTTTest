#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	Components::InitializeEvents();

	auto obj1 = m_scene.Create();
	if (m_scene.Load("scene.json"))
	{
	}
	else
	{
		{
			obj1.AddComponent<Transform>(Transform());
			//obj1.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
			obj1.AddComponent<UpdateRenderer>(UpdateRenderer());
			//obj1.AddTag<entt::tag<"enemy"_hs>>(obj1);
			//obj1.AddTag<int>(4);
			//obj1.AddTag<std::string>(obj1);
		}
		{
			auto obj = m_scene.Create();
			auto transform = Transform();
			transform.position.x = 1;
			obj.AddComponent<Transform>(std::move(transform));
			obj.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
		}
		{
			auto obj = m_scene.Create();
			auto transform = Transform();
			transform.position.x = -1;
			obj.AddComponent<Transform>(std::move(transform));
			obj.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
			obj.AddComponent<MoveUpdater>(MoveUpdater());
			obj.AddComponent<MoveDownUpdater>(MoveDownUpdater());
		}

		m_scene.Save("scene.json");
	}

	auto& reg = m_scene.registry;
	auto& editor = m_context->Register<MM::ImGuiEntityEditor<entt::registry>>();
	// "registerTrivial" registers the type, name, create and destroy functions for trivialy costructable(and destroyable) types.
	// you just need to provide a "widget" function if you use this method.
	Components::InitializeEditorComponents(*m_context, reg, editor);
}

void MyGame::Update()
{
	Updatable::Update(*m_context, m_scene);

	// GUI
	m_context->Get<ImGuiManager>().Update(*m_context);
}

void MyGame::RenderInitialize()
{
	Renderable::RenderInitialize(*m_context, m_scene);
}

void MyGame::Render()
{
	// GUI
	m_context->Get<ImGuiManager>().BeforeRender(*m_context);

	Renderable::Render(*m_context, m_scene);

	m_context->Get<ImGuiManager>().Render(*m_context);

	// render editor
	auto& reg = m_scene.registry;
	auto& editor = m_context->Get<MM::ImGuiEntityEditor<entt::registry>>();
	entt::entity e = 1;
	editor.renderImGui(reg, e);

	// GUI
	m_context->Get<ImGuiManager>().AfterRender(*m_context);
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(*m_context, m_scene);
}

MyGame::~MyGame()
{
}
