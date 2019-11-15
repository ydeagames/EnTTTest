#pragma once

class GameObject;

class Scene final
{
public:
	entt::registry registry;

public:
	GameObject Create();

	bool Load(const std::string& location);
	bool Save(const std::string& location) const;
};

