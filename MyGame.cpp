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
			Transform* transform = nullptr;
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
				node.transform = &transform;
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
		ImGuiTreeNodeFlags node_flags = ((e == entt::null) ? ImGuiTreeNodeFlags_Selected : 0)
			| (nodes.empty() ? ImGuiTreeNodeFlags_Leaf : 0)
			| ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;
		bool opened = ImGui::TreeNodeEx(title.c_str(), node_flags, title.c_str());
		if (ImGui::IsItemClicked())
			e = entt::null;
		if (opened)
		{
			std::stack<entt::entity> stack;
			for (auto& node : nodes)
			{
				if (node.parent == node.id || !(reg.valid(node.parent) && nodes.has(node.parent)))
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
								// Some processing...
								ImGui::EndPopup();
							}
							ImGui::PopID();

							if (ImGui::IsItemClicked())
							{
								e = node.id;
								// Some processing...
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
										node.transform->parent = data;
								}
								ImGui::EndDragDropTarget();
							}

							if (node.parent != entt::null && (node.parent == node.id || !(reg.valid(node.parent) && nodes.has(node.parent))))
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
								if (node.parent != node.id)
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

		{
			ImGui::Begin("Drag Test");
			// Reordering is actually a rather odd use case for the drag and drop API which is meant to carry data around. 
			// Here we implement a little demo using the drag and drop primitives, but we could perfectly achieve the same results by using a mixture of
			//  IsItemActive() on the source item + IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) on target items.
			// This demo however serves as a pretext to demonstrate some of the flags you can use with BeginDragDropSource() and AcceptDragDropPayload().
			ImGui::BulletText("Drag and drop to re-order");
			ImGui::Indent();
			static const char* names[6] = { "1. Adbul", "2. Alfonso", "3. Aline", "4. Amelie", "5. Anna", "6. Arthur" };
			int move_from = -1, move_to = -1;
			for (int n = 0; n < IM_ARRAYSIZE(names); n++)
			{
				ImGui::Selectable(names[n]);

				ImGuiDragDropFlags src_flags = 0;
				src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
				src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
				//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
				if (ImGui::BeginDragDropSource(src_flags))
				{
					if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
						ImGui::Text("Moving \"%s\"", names[n]);
					ImGui::SetDragDropPayload("DND_DEMO_NAME", &n, sizeof(int));
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					ImGuiDragDropFlags target_flags = 0;
					target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
					target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
					if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_DEMO_NAME", target_flags))
					{
						move_from = *(const int*)payload->Data;
						move_to = n;
					}
					ImGui::EndDragDropTarget();
				}
			}

			if (move_from != -1 && move_to != -1)
			{
				// Reorder items
				int copy_dst = (move_from < move_to) ? move_from : move_to + 1;
				int copy_src = (move_from < move_to) ? move_from + 1 : move_to;
				int copy_count = (move_from < move_to) ? move_to - move_from : move_from - move_to;
				const char* tmp = names[move_from];
				//printf("[%05d] move %d->%d (copy %d..%d to %d..%d)\n", ImGui::GetFrameCount(), move_from, move_to, copy_src, copy_src + copy_count - 1, copy_dst, copy_dst + copy_count - 1);
				memmove(&names[copy_dst], &names[copy_src], (size_t)copy_count * sizeof(const char*));
				names[move_to] = tmp;
				ImGui::SetDragDropPayload("DND_DEMO_NAME", &move_to, sizeof(int)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
			}
			ImGui::Unindent();
			ImGui::End();
		}
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
