#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"

struct EntityEditorState
{
	entt::entity current;
};

MyGame::MyGame(GameContext* context)
	: m_context(context)
{
	Components::InitializeEvents();
	Components::InitializeDependency(m_scene.registry);

	m_scene.name = "scene";
	m_scene.location = m_scene.name + ".json";

	auto obj1 = m_scene.Create();
	//if (m_scene.Load())
	//{
	//}
	//else
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
			obj.AddComponent<PrimitiveRenderer>();
			//obj.AddComponent<Transform>(std::move(transform));
			//obj.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
		}
		//{
		//	auto obj = m_scene.Create();
		//	auto transform = Transform();
		//	transform.position.x = -1;
		//	obj.AddComponent<Transform>(std::move(transform));
		//	obj.AddComponent<PrimitiveRenderer>(PrimitiveRenderer());
		//	obj.AddComponent<MoveUpdater>(MoveUpdater());
		//	obj.AddComponent<MoveDownUpdater>(MoveDownUpdater());
		//}

		m_scene.Save();
	}

	auto& reg = m_scene.registry;
	auto& editor = m_context->Register<MM::ImGuiEntityEditor<entt::registry>>();
	// "registerTrivial" registers the type, name, create and destroy functions for trivialy costructable(and destroyable) types.
	// you just need to provide a "widget" function if you use this method.
	Components::InitializeEditorComponents(*m_context, reg, editor);

	m_context->Register<EntityEditorState>();
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
	auto& editorState = m_context->Get<EntityEditorState>();
	editor.renderImGui(reg, editorState.current);

	if (ImGui::Begin("Hierarchy"))
	{
		auto& e = editorState.current;
		entt::entity id = reg.entity(e);
		int iid = int(id);
		ImGui::InputInt("id", &iid);
		id = reg.entity(entt::entity(iid));
		e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;

		if (ImGui::Button("New Entity")) {
			auto prev = e;
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(prev))
				t.parent = prev;
			reg.assign<Transform>(e0, std::move(t));
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Entity")) {
			reg.destroy(e);
		}

		ImGui::Separator();

		class Node
		{
		public:
			entt::entity id;
			std::string name;
			entt::entity parent = entt::null;
			std::vector<entt::entity> children;

		public:
			Node(const entt::entity& id)
				: id(id)
			{}
		};

		entt::SparseSet<entt::entity, Node> nodes;
		reg.each([&](auto entity) {
			Node node{ entity };

			std::stringstream sb;
			sb << "[" << reg.entity(entity) << "]";
			if (reg.has<Transform>(entity))
			{
				auto& transform = reg.get<Transform>(entity);
				node.parent = transform.parent;
				sb << " " << transform.name;
			}
			node.name = sb.str();
			nodes.construct(entity, std::move(node));
			});

		for (auto& node : nodes)
		{
			if (reg.valid(node.parent) && nodes.has(node.parent))
				nodes.get(node.parent).children.push_back(node.id);
		}

		std::string title = "Scene [" + m_scene.name + "]";
		if (ImGui::CollapsingHeader(title.c_str()))
		{
			std::stack<entt::entity> stack;
			for (auto& node : nodes)
			{
				bool hasparent = reg.valid(node.parent) && nodes.has(node.parent);
				if (!hasparent)
				{
					auto rec0 = [&](Node& node, auto& rec) mutable -> void {
						ImGui::Indent(-5.f);
						if (node.children.empty())
						{
							ImGui::Indent(20.f);
							ImGui::Text(node.name.c_str());
							ImGui::Unindent(20.f);
						}
						else
						{
							if (ImGui::TreeNode(node.name.c_str()))
							{
								for (auto& nodeindex : node.children)
								{
									rec(nodes.get(nodeindex), rec);
								}
								ImGui::TreePop();
							}
						}
						ImGui::Unindent(-5.f);
					};
					rec0(node, rec0);
				}
			}
		}

		//std::vector<const char*> items;
		//for (auto& node : nodes)
		//	items.push_back(node.name.c_str());

		//int current = int(nodeindex.get(editorState.current));
		//ImGui::ListBox("Hierarchy", &current, items.data(), items.size());
		//editorState.current = nodes[current].id;
		ImGui::End();
	}

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
