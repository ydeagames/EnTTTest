#include "pch.h"
#include "Widgets.h"
#include "Scene.h"
#include "GameContext.h"
#include "AllComponents.h"
#include "WindowsUtils.h"

namespace Widgets
{
	void Hierarchy(GameContext& ctx, Scene& scene)
	{
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

		auto& reg = scene.registry;
		auto& editorState = ctx.Get<EntityEditorState>();
		auto& e = editorState.current;

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

		std::string title = "Scene [" + scene.name + "]";
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
	}

	void Inspector(GameContext& ctx, Scene& scene)
	{
		auto& reg = scene.registry;
		auto& editor = ctx.Get<MM::ImGuiEntityEditor<entt::registry>>();
		auto& editorState = ctx.Get<EntityEditorState>();
		editor.renderImGui(reg, editorState.current);
	}

	void SceneControl(GameContext& ctx, Scene& scene)
	{
		ImGui::LabelText("Scene", scene.name.c_str());

		if (ImGui::Button("Save Scene"))
		{
			scene.Save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			scene.Load();
		}
		if (ImGui::Button("Save Scene As"))
		{
			std::string location;
			if (WindowsUtils::SaveDialog("scene.json", "Scene Files", location))
			{
				scene.location = location;
				scene.name = WindowsUtils::GetFileName(location, "scene.json");
				scene.Save();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Scene"))
		{
			std::string location;
			if (WindowsUtils::OpenDialog("scene.json", "Scene Files", location))
			{
				scene.location = location;
				scene.name = WindowsUtils::GetFileName(location, "scene.json");
				scene.Load();
			}
		}
	}

	void EntityControl(GameContext& ctx, Scene& scene)
	{
		auto& reg = scene.registry;
		auto& editorState = ctx.Get<EntityEditorState>();
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

		if (ImGui::Button("New"))
		{
			auto prev = e;
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(prev))
			{
				auto parent = reg.has<Transform>(prev) ? reg.get<Transform>(prev).parent : entt::null;
				if (reg.valid(parent))
					t.parent = parent;
			}
			reg.assign<Transform>(e0, std::move(t));
			if (!ImGui::GetIO().KeyShift)
				e = e0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			auto rec0 = [&](auto& e, auto& rec) mutable -> void {
				reg.view<Transform>().each([&](auto entity, Transform& component) {
					if (component.parent == e)
						rec(entity, rec);
					});
				reg.destroy(e);
			};
			rec0(e, rec0);
		}
		if (ImGui::Button("New Child"))
		{
			auto prev = e;
			auto e0 = reg.create();
			Transform t;
			if (reg.valid(prev))
				t.parent = prev;
			reg.assign<Transform>(e0, std::move(t));
			if (!ImGui::GetIO().KeyShift)
				e = e0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Duplicate"))
		{
			auto prev = e;
			if (reg.valid(prev))
			{
				std::vector<entt::entity> src;
				std::vector<entt::entity> dst;
				auto rec0 = [&](auto& e, auto& rec) mutable -> void {
					reg.view<Transform>().each([&](auto entity, Transform& component) {
						if (component.parent == e)
							rec(entity, rec);
						});
					src.push_back(e);
					dst.push_back(reg.create());
				};
				rec0(e, rec0);

				Components::CloneComponents(reg, src, dst);
				Components::UpdateReferences(reg, src, dst);

				if (!ImGui::GetIO().KeyShift)
					e = *(dst.end() - 1);
			}
		}
		if (ImGui::Button("Export"))
		{
			std::string location;
			if (WindowsUtils::SaveDialog("prefab.json", "Prefab Files", location))
			{
				Components::SaveEntity(location, reg, e);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Import"))
		{
			std::string location;
			if (WindowsUtils::OpenDialog("prefab.json", "Prefab Files", location))
			{
				auto prev = e;
				auto e0 = reg.create();
				Components::LoadEntity(location, reg, e0);
				auto& t = reg.get_or_assign<Transform>(e0);
				if (reg.valid(prev))
					t.parent = prev;
				if (!ImGui::GetIO().KeyShift)
					e = e0;
			}
		}
	}

	namespace AllWidgets
	{
		void Initialize(GameContext& ctx, Scene& scene)
		{
			auto& reg = scene.registry;
			ctx.Register<EntityEditorState>();
			auto& editor = ctx.Register<MM::ImGuiEntityEditor<entt::registry>>();
			Components::InitializeEditorComponents(ctx, reg, editor);
		}

		void Render(GameContext& ctx, Scene& scene)
		{
			Widgets::Inspector(ctx, scene);

			if (ImGui::Begin("Hierarchy"))
			{
				Widgets::SceneControl(ctx, scene);

				ImGui::Separator();

				Widgets::EntityControl(ctx, scene);

				ImGui::Separator();

				Widgets::Hierarchy(ctx, scene);

				ImGui::End();
			}
		}
	}
}
