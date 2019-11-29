#pragma once
#include "Components.h"

class MyGame
{
private:
	Scene m_scene;

public:
	MyGame();
	void Update();
	void RenderInitialize();
	void Render(Camera& camera);
	void RenderFinalize();
	~MyGame();

	int Bench();
};
