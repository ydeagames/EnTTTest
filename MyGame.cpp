#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"
#include "Widgets.h"

int MyGame::Bench()
{
	/*
	std::cout << "Performance Test" << std::endl;
	//DirectX::SimpleMath::Matrix m1 = DirectX::SimpleMath::Matrix::CreateRotationX(.5f);
	//DirectX::SimpleMath::Matrix m2 = DirectX::SimpleMath::Matrix::CreateRotationY(.5f);
	Renderable::RenderInitialize(*m_context, m_scene);
	Renderable::Render(*m_context, m_scene);
	auto t1 = std::chrono::high_resolution_clock::now();
	constexpr long count = 100'000L;
	for (long i = 0; i < count; i++)
	{
		//reg.get<Transform>(0);
		//m1* m2;
		Renderable::Render(*m_context, m_scene);
		//Updatable::Update(*m_context, m_scene);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Finish in " << duration / 1'000'000.f << " (" << duration / (float)count << "��s)" << std::endl;
	*/
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
	// ImGui�R���e�L�X�g
	auto imgui = m_context->Register<ImGuiManager>();

	// ImGui������
	imgui.RenderInitialize(*m_context);

	Renderable::RenderInitialize(*m_context, m_scene);
}

void MyGame::Render()
{
	static int bench = Bench();

	// �`��C�x���g
	Renderable::Render(*m_context, m_scene);

	// ImGui
	{
		// ImGui�R���e�L�X�g
		auto& imgui = m_context->Get<ImGuiManager>();

		// ImGui�`��J�n
		imgui.Begin(*m_context);

		// Widgets
		Widgets::AllWidgets::Render(*m_context, m_scene);

		// ImGui�`��I��
		imgui.End(*m_context);
	}
}

void MyGame::RenderFinalize()
{
	Renderable::RenderFinalize(*m_context, m_scene);

	// ImGui�R���e�L�X�g
	auto& imgui = m_context->Get<ImGuiManager>();

	// ImGui�t�@�C�i���C�Y
	imgui.RenderFinalize(*m_context);

	// ImGui�폜
	m_context->Remove<ImGuiManager>();
}

MyGame::~MyGame()
{
}
