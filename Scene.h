#pragma once

class GameObject;

class Scene final
{
public:
	std::string name;
	std::string location;
	entt::registry registry;

public:
	GameObject Create();

	bool Load();
	bool Save() const;
};

