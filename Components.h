#pragma once
#include "Events.h"
#include "DeviceResources.h"
#include "GameContext.h"
#include "Component.h"

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

class Component
{
public:
	static constexpr const char* Identifier = "Component";
	GameObject gameObject;

//public:
//	Component() = default;
//	~Component() = default;
//	Component(const Component& c) : gameObject(c.gameObject) { std::cout << "Copy Constructor" << std::endl; }
//	Component& operator=(const Component& c) { std::cout << "Copy Assignment" << std::endl; gameObject = c.gameObject; return *this; }
};

class Transform : public Component
{
public:
	static constexpr const char* Identifier = "Transform";

public:
	std::string name;
	entt::entity parent = entt::null;
	//bool isStatic = false;

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
		archive(CEREAL_OPTIONAL_NVP(name), CEREAL_OPTIONAL_NVP(parent)/*, CEREAL_OPTIONAL_NVP(isStatic)*/);
		archive(CEREAL_OPTIONAL_NVP(position), CEREAL_OPTIONAL_NVP(rotation), CEREAL_OPTIONAL_NVP(scale));
	}

	template<class Reference>
	void Reference(Reference& reference)
	{
		reference(parent);
	}

	void EditorGui();
};

class MoveUpdater : public Component
{
public:
	static constexpr const char* Identifier = "MoveUpdater";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	DirectX::SimpleMath::Vector3 vel;

public:
	void Start();
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class MoveDownUpdater : public Component
{
public:
	static constexpr const char* Identifier = "MoveDownUpdater";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	void Update();

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class PrimitiveRenderer : public Component
{
public:
	static constexpr const char* Identifier = "PrimitiveRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<Transform>();
	}

public:
	std::unique_ptr<DirectX::GeometricPrimitive> m_model;

public:
	void Awake() { std::cout << "Awake" << gameObject.entity << std::endl; }
	void OnDestroy() { std::cout << "OnDestroy" << gameObject.entity << std::endl; }
	void RenderStart();
	void Render(Camera& camera);

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};

class UpdateRenderer : public MoveUpdater
{
public:
	static constexpr const char* Identifier = "UpdateRenderer";

	template<typename Component>
	static void Dependency(Component& component)
	{
		component.DependsOn<MoveUpdater, Transform, PrimitiveRenderer>();
	}

public:
	template<class Archive>
	void serialize(Archive& archive)
	{
	}
};