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

MyGame::MyGame()
{
	Components::InitializeEvents();

	m_scene.name = "scene";
	m_scene.location = m_scene.name + ".json";

	m_scene.Load();

	// Widgets
	Widgets::AllWidgets::Initialize(m_scene);
}

void MyGame::Update()
{
	Updatable::Update(m_scene);
}

void MyGame::RenderInitialize()
{
	// ImGuiコンテキスト
	auto imgui = GameContext::Register<ImGuiManager>();

	// ImGui初期化
	imgui.RenderInitialize();

	Renderable::RenderInitialize(m_scene);
}

void MyGame::Render(Camera& camera)
{
	static int bench = Bench();

	// 描画イベント
	Renderable::Render(m_scene, std::forward<Camera>(camera));

	// ImGui
	{
		// ImGuiコンテキスト
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui描画開始
		imgui.Begin();

		// Widgets
		Widgets::AllWidgets::Render(m_scene);

		// ImGui描画終了
		imgui.End();
	}
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(m_scene);

	// ImGuiコンテキスト
	auto& imgui = GameContext::Get<ImGuiManager>();

	// ImGuiファイナライズ
	imgui.RenderFinalize();

	// ImGui削除
	GameContext::Remove<ImGuiManager>();
}

MyGame::~MyGame()
{
}
