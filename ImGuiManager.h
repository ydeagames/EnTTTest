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
	// ����
	void RenderInitialize();
	// �`��
	void Begin();
	void End();
	// �j��
	void RenderFinalize();
};
