#pragma once
#include "Components.h"

class MyGame
{
private:
	entt::DefaultRegistry m_scene;
	GameContext* m_context;

public:
	MyGame(GameContext* context);
	void Update();
	void RenderInitialize();
	void Render();
	void RenderFinalize();
	~MyGame();
};
