#pragma once

class ISceneBuilder;
class GameContext;

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	std::string m_settingFile;

public:
	// ê∂ê¨
	void RenderInitialize(GameContext& context);
	// ï`âÊ
	void Begin(GameContext& context);
	void End(GameContext& context);
	// îjä¸
	void RenderFinalize(GameContext& context);
};
