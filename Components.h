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
	static constexpr const char* ComponentName = "Transform";

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
	static constexpr const char* ComponentName = "MoveUpdater";

public:
	void Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class MoveDownUpdater
{
public:
	static constexpr const char* ComponentName = "MoveDownUpdater";

public:
	void Update(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class PrimitiveRenderer
{
public:
	static constexpr const char* ComponentName = "PrimitiveRenderer";

public:
	std::shared_ptr<DirectX::GeometricPrimitive> m_model;

public:
	void RenderInitialize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
	void Render(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);
	void RenderFinalize(GameContext& ctx, entt::DefaultRegistry& registry, entt::DefaultRegistry::entity_type entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class UpdateRenderer : public MoveUpdater, public PrimitiveRenderer
{
public:
	static constexpr const char* ComponentName = "UpdateRenderer";

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};