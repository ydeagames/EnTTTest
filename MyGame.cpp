#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"

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

	m_scene.Load();

	/*
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
	*/

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
	static int bench = Bench();

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
		{
			int iid = (e == entt::null) ? -1 : int(reg.entity(e));
			ImGui::InputInt("ID", &iid);
			if (iid < 0)
				e = entt::null;
			else
			{
				auto id = entt::entity(iid);
				e = id < reg.size() ? (id | reg.current(id) << entt::entt_traits<entt::entity>::entity_shift) : id;
			}
		}

		if (ImGui::Button("New Entity")) {
			auto prev = e;
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(prev))
				t.parent = prev;
			reg.assign<Transform>(e0, std::move(t));
			e = e0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Entity")) {
			auto rec0 = [&](auto& e, auto& rec) mutable -> void {
				reg.view<Transform>().each([&](auto entity, Transform& component) {
					if (component.parent == e)
						rec(entity, rec);
					});
				reg.destroy(e);
			};
			rec0(e, rec0);
		}
		if (ImGui::Button("Save Scene")) {
			m_scene.Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene")) {
			m_scene.Load();
		}

		ImGui::Separator();

		class Node
		{
		public:
			entt::entity id;
			std::string name;
			entt::entity parent = entt::null;
			bool hasloop = false;
			bool hasparent = false;
			Transform* transform = nullptr;
			std::vector<entt::entity> children;

		public:
			Node(const entt::entity& id)
				: id(id)
			{}
		};

		//std::unordered_set<entt::entity> checknodes;
		entt::SparseSet<entt::entity, Node> nodes;
		reg.each([&](auto entity) {
			Node node{ entity };

			std::stringstream sb;
			sb << "[" << reg.entity(entity) << "]";
			if (reg.has<Transform>(entity))
			{
				auto& transform = reg.get<Transform>(entity);
				node.transform = &transform;
				node.parent = transform.parent;
				sb << " " << transform.name;
			}
			node.name = sb.str();
			nodes.construct(entity, std::move(node));
			});

		for (auto& node : nodes)
		{
			auto loop = [&nodes](entt::entity id) {
				auto slow = id, fast = id;
				while (true)
				{
					//if (!nodes.has(slow))
					//	return false;
					slow = nodes.get(slow).parent;
					//if (!nodes.has(fast))
					//	return false;
					fast = nodes.get(fast).parent;
					if (!nodes.has(fast))
						return false;
					fast = nodes.get(fast).parent;
					if (!nodes.has(slow) || !nodes.has(fast))
						return false;
					if (slow == fast)
						return true;
				}
			};
			node.hasparent = reg.valid(node.parent) && nodes.has(node.parent);
			if (node.id == node.parent || loop(node.id))
				node.hasloop = true;
			else if (reg.valid(node.parent) && nodes.has(node.parent))
				nodes.get(node.parent).children.push_back(node.id);
		}

		std::string title = "Scene [" + m_scene.name + "]";
		ImGuiTreeNodeFlags node_flags = ((e == entt::null) ? ImGuiTreeNodeFlags_Selected : 0)
			| (nodes.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;
		bool opened = ImGui::TreeNodeEx(title.c_str(), node_flags, title.c_str());

		if (ImGui::IsItemClicked())
			e = entt::null;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
			{
				auto data = *(static_cast<const entt::entity*>(payload->Data));
				reg.get<Transform>(data).parent = entt::null;
			}
			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (auto& node : nodes)
			{
				if (node.hasloop || !node.hasparent)
				{
					auto rec0 = [&](Node& node, auto& rec) mutable -> void {
						ImGui::Indent(-5.f);
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
							ImGuiTreeNodeFlags node_flags = ((e == node.id) ? ImGuiTreeNodeFlags_Selected : 0)
								| (node.children.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
								| ImGuiTreeNodeFlags_OpenOnArrow
								| ImGuiTreeNodeFlags_OpenOnDoubleClick
								| ImGuiTreeNodeFlags_DefaultOpen;
							bool opened = ImGui::TreeNodeEx(node.name.c_str(), node_flags, node.name.c_str());
							ImGui::PopStyleVar();

							ImGui::PushID(node.name.c_str());
							if (ImGui::BeginPopupContextItem())
							{
								ImGui::EndPopup();
							}
							ImGui::PopID();

							if (ImGui::IsItemClicked())
							{
								e = node.id;
							}

							if (ImGui::BeginDragDropSource())
							{
								ImGui::SetDragDropPayload("DND_Hierarchy", &node.id, sizeof(entt::entity));
								ImGui::Text(node.name.c_str());
								ImGui::EndDragDropSource();
							}

							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_Hierarchy"))
								{
									auto data = *(static_cast<const entt::entity*>(payload->Data));
									if (data != node.id && data != node.parent)
										reg.get<Transform>(data).parent = node.id;
								}
								ImGui::EndDragDropTarget();
							}

							if (node.parent != entt::null && (node.hasloop || !node.hasparent))
							{
								if (node.transform)
								{
									ImGui::SameLine();
									if (ImGui::SmallButton("Fix Transform"))
										node.transform->parent = entt::null;
								}
							}

							if (opened)
							{
								if (!node.hasloop)
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
			ImGui::TreePop();
		}

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
