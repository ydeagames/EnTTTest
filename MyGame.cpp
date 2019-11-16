#include "pch.h"
#include "MyGame.h"
#include "Components.h"
#include "Serialize.h"
#include "AllComponents.h"
#include "ImGuiManager.h"

DirectX::SimpleMath::Quaternion ToQuaternion(const DirectX::SimpleMath::Vector3& euler) // yaw (Z), pitch (Y), roll (X)
{
	// Abbreviations for the various angular functions
	float cy = std::cos(euler.z * 0.5f);
	float sy = std::sin(euler.z * 0.5f);
	float cp = std::cos(euler.y * 0.5f);
	float sp = std::sin(euler.y * 0.5f);
	float cr = std::cos(euler.x * 0.5f);
	float sr = std::sin(euler.x * 0.5f);

	DirectX::SimpleMath::Quaternion q;
	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * cp * sr - sy * sp * cr;
	q.y = sy * cp * sr + cy * sp * cr;
	q.z = sy * cp * cr - cy * sp * sr;

	return q;
}

DirectX::SimpleMath::Vector3 ToEulerAngles(const DirectX::SimpleMath::Quaternion& q) {
	DirectX::SimpleMath::Vector3 angles;

	// roll (x-axis rotation)
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		angles.y = std::copysign(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
	else
		angles.y = std::asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}

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
	editor.registerTrivial<Transform>(reg, "Transform");

	editor.registerComponentWidgetFn(
		reg.type<Transform>(),
		[](entt::registry& reg, entt::registry::entity_type e) {
			auto& t = reg.get<Transform>(e);

			if (ImGui::CollapsingHeader("Position"))
			{
				// the "##Transform" ensures that you can use the name "x" in multiple lables
				ImGui::DragFloat("x##Transform.Position", &t.position.x, 0.1f);
				ImGui::DragFloat("y##Transform.Position", &t.position.y, 0.1f);
				ImGui::DragFloat("z##Transform.Position", &t.position.z, 0.1f);
			}
			if (ImGui::CollapsingHeader("Rotation"))
			{
				auto euler = ToEulerAngles(DirectX::SimpleMath::Quaternion(t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w)) * (180.f / DirectX::XM_PI);

				ImGuiStorage* storage = ImGui::GetStateStorage();
				float* x = storage->GetFloatRef(ImGuiID("Transform.Rotation.x"_hs), euler.x);
				float* y = storage->GetFloatRef(ImGuiID("Transform.Rotation.y"_hs), euler.y);
				float* z = storage->GetFloatRef(ImGuiID("Transform.Rotation.z"_hs), euler.z);
				float* qx = storage->GetFloatRef(ImGuiID("Transform.Rotation.qx"_hs), t.rotation.x);
				float* qy = storage->GetFloatRef(ImGuiID("Transform.Rotation.qy"_hs), t.rotation.y);
				float* qz = storage->GetFloatRef(ImGuiID("Transform.Rotation.qz"_hs), t.rotation.z);
				float* qw = storage->GetFloatRef(ImGuiID("Transform.Rotation.qw"_hs), t.rotation.w);
				bool* edit = storage->GetBoolRef(ImGuiID("Transform.Rotation.edit"_hs), false);

				if (!*edit)
				{
					*x = euler.x;
					*y = euler.y;
					*z = euler.z;
					*qx = t.rotation.x;
					*qy = t.rotation.y;
					*qz = t.rotation.z;
					*qw = t.rotation.w;
				}

				// the "##Transform" ensures that you can use the name "x" in multiple lables
				ImGui::DragFloat("x##Transform.Rotation", x, 0.1f);
				ImGui::DragFloat("y##Transform.Rotation", y, 0.1f);
				ImGui::DragFloat("z##Transform.Rotation", z, 0.1f);

				if (*edit)
				{
					auto quat = ToQuaternion(DirectX::SimpleMath::Vector3(*x, *y, *z) * (DirectX::XM_PI / 180.f));
					t.rotation.x = quat.x;
					t.rotation.y = quat.y;
					t.rotation.z = quat.z;
					t.rotation.w = quat.w;
				}

				if (ImGui::Button(*edit ? "Save" : "Edit"))
				{
					*edit = !*edit;
				}
				if (ImGui::Button("Discard"))
				{
					t.rotation.x = *qx;
					t.rotation.y = *qy;
					t.rotation.z = *qz;
					t.rotation.w = *qw;
					*edit = false;
				}
			}
			if (ImGui::CollapsingHeader("Scale"))
			{
				// the "##Transform" ensures that you can use the name "x" in multiple lables
				ImGui::DragFloat("x##Transform.Scale", &t.scale.x, 0.1f);
				ImGui::DragFloat("y##Transform.Scale", &t.scale.y, 0.1f);
				ImGui::DragFloat("z##Transform.Scale", &t.scale.z, 0.1f);
			}
		});
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
