#pragma once

class GameContext;
class Scene;

namespace Widgets
{
	class EntityEditorState
	{
	public:
		entt::entity current;
	};

	void Hierarchy(GameContext& ctx, Scene& scene);
	void Inspector(GameContext& ctx, Scene& scene);
	void SceneControl(GameContext& ctx, Scene& scene);
	void EntityControl(GameContext& ctx, Scene& scene);

	namespace AllWidgets
	{
		void Initialize(GameContext& ctx, Scene& scene);
		void Render(GameContext& ctx, Scene& scene);
	}
};
