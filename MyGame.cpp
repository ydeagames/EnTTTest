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
	//	std::cout << "Finish in " << duration / 1'000'000.f << " (" << duration / (float)count << "��s)" << std::endl;
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
	// ImGui�R���e�L�X�g
	auto imgui = GameContext::Register<ImGuiManager>();

	// ImGui������
	imgui.RenderInitialize();

	Renderable::RenderInitialize(m_scene);
}

void MyGame::Render(Camera& camera)
{
	static int bench = Bench();

	// �`��C�x���g
	Renderable::Render(m_scene, std::forward<Camera>(camera));

	// ImGui
	{
		// ImGui�R���e�L�X�g
		auto& imgui = GameContext::Get<ImGuiManager>();

		// ImGui�`��J�n
		imgui.Begin();

		// Widgets
		Widgets::AllWidgets::Render(m_scene);

		// ImGui�`��I��
		imgui.End();
	}
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(m_scene);

	// ImGui�R���e�L�X�g
	auto& imgui = GameContext::Get<ImGuiManager>();

	// ImGui�t�@�C�i���C�Y
	imgui.RenderFinalize();

	// ImGui�폜
	GameContext::Remove<ImGuiManager>();
}

MyGame::~MyGame()
{
}
