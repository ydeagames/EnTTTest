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
	// �X�V
	void Update(GameContext& context);
	// �`��
	void BeforeRender(GameContext& context);
	void Render(GameContext& context);
	void AfterRender(GameContext& context);
	// �j��
	void Finalize(GameContext& context);
};
