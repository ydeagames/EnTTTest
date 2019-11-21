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
	void Initialize(GameContext& context);
	// �`��
	void Begin(GameContext& context);
	void End(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};
