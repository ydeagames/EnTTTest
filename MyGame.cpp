#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"
#include "Widgets.h"

int MyGame::Bench()
{
	//constexpr long count = 100'000'000L;
	//{
	//	std::cout << "Performance Test" << std::endl;
	//	auto t1 = std::chrono::high_resolution_clock::now();
	//	for (long i = 0; i < count; i++)
	//	{
	//		auto& camera = GameContext::Get<Camera>();
	//	}
	//	auto t2 = std::chrono::high_resolution_clock::now();
	//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	//	std::cout << "Finish in " << duration / 1'000'000.f << " (" << duration / (float)count << "μs)" << std::endl;
	//}
	return 0;
}

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	Components::InitializeEvents();

	m_scene.name = "scene";
	m_scene.location = m_scene.name + ".json";

	m_scene.Load();

	// Widgets
	Widgets::AllWidgets::Initialize(*m_context, m_scene);
}

void MyGame::Update()
{
	Updatable::Update(*m_context, m_scene);
}

void MyGame::RenderInitialize()
{
	// ImGuiコンテキスト
	auto imgui = m_context->Register<ImGuiManager>();

	// ImGui初期化
	imgui.RenderInitialize(*m_context);

	Renderable::RenderInitialize(*m_context, m_scene);
}

void MyGame::Render()
{
	static int bench = Bench();

	// 描画イベント
	Renderable::Render(*m_context, m_scene);

	// ImGui
	{
		// ImGuiコンテキスト
		auto& imgui = m_context->Get<ImGuiManager>();

		// ImGui描画開始
		imgui.Begin(*m_context);

		// Widgets
		Widgets::AllWidgets::Render(*m_context, m_scene);

		// ImGui描画終了
		imgui.End(*m_context);
	}
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(*m_context, m_scene);

	// ImGuiコンテキスト
	auto& imgui = m_context->Get<ImGuiManager>();

	// ImGuiファイナライズ
	imgui.RenderFinalize(*m_context);

	// ImGui削除
	m_context->Remove<ImGuiManager>();
}

MyGame::~MyGame()
{
}
