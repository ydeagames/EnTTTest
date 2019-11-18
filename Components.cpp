#include "pch.h"
#include "Components.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PrimitiveRenderer::RenderStart(GameContext& ctx, GameObject& entity)
{
	m_model = GeometricPrimitive::CreateTeapot(ctx.dr->GetD3DDeviceContext());
}

void PrimitiveRenderer::Render(GameContext& ctx, GameObject& entity)
{
	if (m_model)
		m_model->Draw(entity.GetComponent<Transform>().GetMatrix(), ctx.view, ctx.projection);
}

void MoveUpdater::Start(GameContext& ctx, GameObject& entity)
{
	vel += DirectX::SimpleMath::Vector3::Right * .1f;
}

void MoveUpdater::Update(GameContext& ctx, GameObject& entity)
{
	entity.GetComponent<Transform>().position += vel;
}

void MoveDownUpdater::Update(GameContext& ctx, GameObject& entity)
{
	entity.GetComponent<Transform>().position.y -= .1f;
}

namespace
{
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
}

void Transform::EditorGui(GameContext& ctx, GameObject& entity)
{
	auto& t = entity.GetComponent<Transform>();

	std::string tmpname = t.name;
	tmpname.resize(16);
	ImGui::InputText("Name##Transform", &tmpname[0], tmpname.size());
	t.name = tmpname;

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

		float x = euler.x;
		float y = euler.y;
		float z = euler.z;

		// the "##Transform" ensures that you can use the name "x" in multiple lables
		ImGui::DragFloat("x##Transform.Rotation", &x, 0.1f);
		ImGui::DragFloat("y##Transform.Rotation", &y, 0.1f);
		ImGui::DragFloat("z##Transform.Rotation", &z, 0.1f);

		auto quat = ToQuaternion(DirectX::SimpleMath::Vector3(x, y, z) * (DirectX::XM_PI / 180.f));
		t.rotation.x = quat.x;
		t.rotation.y = quat.y;
		t.rotation.z = quat.z;
		t.rotation.w = quat.w;
	}
	if (ImGui::CollapsingHeader("Scale"))
	{
		// the "##Transform" ensures that you can use the name "x" in multiple lables
		ImGui::DragFloat("x##Transform.Scale", &t.scale.x, 0.1f);
		ImGui::DragFloat("y##Transform.Scale", &t.scale.y, 0.1f);
		ImGui::DragFloat("z##Transform.Scale", &t.scale.z, 0.1f);
	}
}