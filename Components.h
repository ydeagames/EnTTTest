#pragma once
#include "Events.h"
#include "DeviceResources.h"

namespace DirectX
{
	namespace SimpleMath
	{
		template<typename Archive>
		void serialize(Archive& archive, Vector3& vector) {
			archive(cereal::make_nvp("x", vector.x), cereal::make_nvp("y", vector.y), cereal::make_nvp("z", vector.z));
		}

		template<typename Archive>
		void serialize(Archive& archive, Quaternion& quat) {
			archive(cereal::make_nvp("x", quat.x), cereal::make_nvp("y", quat.y), cereal::make_nvp("z", quat.z), cereal::make_nvp("w", quat.w));
		}
	}
}

class GameContext
{
public:
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;

public:
	DX::DeviceResources* dr = nullptr;
};

class Transform
{
public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 scale = { 1, 1, 1 };

public:
	DirectX::SimpleMath::Matrix GetMatrix()
	{
		return DirectX::SimpleMath::Matrix::CreateTranslation(position)
			* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
			* DirectX::SimpleMath::Matrix::CreateScale(scale);
	}

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
	}
};

class MoveUpdater
{
public:
	MoveUpdater() { Updatable::Register<MoveUpdater>(); }
	void Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
};

class MoveDownUpdater
{
public:
	MoveDownUpdater() { Updatable::Register<MoveDownUpdater>(); }
	void Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
};

class PrimitiveRenderer
{
public:
	std::shared_ptr<DirectX::GeometricPrimitive> m_model;

public:
	PrimitiveRenderer() { Renderable::Register<PrimitiveRenderer>(); }
	void RenderInitialize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
	void Render(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
	void RenderFinalize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		//archive;
	}
};

class UpdateRenderer : public MoveUpdater, public PrimitiveRenderer
{
public:
	UpdateRenderer()
	{
		Updatable::Register<UpdateRenderer>();
		Renderable::Register<UpdateRenderer>();
	}
};