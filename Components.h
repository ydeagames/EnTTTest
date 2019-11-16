#pragma once
#include "Events.h"
#include "DeviceResources.h"
#include "GameContext.h"

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

class Transform
{
public:
	static constexpr const char* Identifier = "Transform";

public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 scale = { 1, 1, 1 };

public:
	DirectX::SimpleMath::Matrix GetMatrix()
	{
		return DirectX::SimpleMath::Matrix::CreateScale(scale)
			* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
			* DirectX::SimpleMath::Matrix::CreateTranslation(position);
	}

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
	}

	void EditorGui(GameContext& ctx, GameObject& entity);
};

class MoveUpdater
{
public:
	static constexpr const char* Identifier = "MoveUpdater";

public:
	void Update(GameContext& ctx, GameObject& entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class MoveDownUpdater
{
public:
	static constexpr const char* Identifier = "MoveDownUpdater";

public:
	void Update(GameContext& ctx, GameObject& entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class PrimitiveRenderer
{
public:
	static constexpr const char* Identifier = "PrimitiveRenderer";

public:
	std::shared_ptr<DirectX::GeometricPrimitive> m_model;

public:
	void RenderInitialize(GameContext& ctx, GameObject& entity);
	void Render(GameContext& ctx, GameObject& entity);
	void RenderFinalize(GameContext& ctx, GameObject& entity);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class UpdateRenderer : public MoveUpdater, public PrimitiveRenderer
{
public:
	static constexpr const char* Identifier = "UpdateRenderer";

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};